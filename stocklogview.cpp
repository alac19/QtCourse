#include "stocklogview.h"
#include "ui_stocklogview.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>

StockLogView::StockLogView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StockLogView),
    logModel(nullptr),
    proxyModel(nullptr)
{
    ui->setupUi(this);

    // 初始化界面
    setupTableView();

    // 加载数据
    loadLogData();
}

StockLogView::~StockLogView()
{
    delete ui;
    delete proxyModel;
    delete logModel;  // 确保删除模型
}

void StockLogView::on_btSearch_clicked()
{
    if (!proxyModel || !logModel) {
        QMessageBox::warning(this, "警告", "库存流水表未初始化");
        return;
    }

    QString keyword = ui->txtSearch->text().trimmed();

    if (keyword.isEmpty()) {
        // 清空过滤
        proxyModel->setFilterFixedString("");
    } else {
        // 构建过滤条件
        proxyModel->setFilterFixedString(keyword);
        proxyModel->setFilterKeyColumn(-1); // 在所有列中搜索
    }

    // 更新统计信息
    int filteredRecords = proxyModel->rowCount();
    int totalRecords = logModel->rowCount();
    ui->lblSummary->setText(QString("共 %1 条记录 (筛选后：%2)").arg(totalRecords).arg(filteredRecords));
}


void StockLogView::on_btRefresh_clicked()
{
    loadLogData();
    QMessageBox::information(this, "刷新", "库存流水数据已刷新");
}


void StockLogView::on_btExport_clicked()
{
    if (!proxyModel || !logModel) {
        QMessageBox::warning(this, "警告", "没有数据可以导出");
        return;
    }

    // 导出功能
    QString fileName = QFileDialog::getSaveFileName(this, "导出库存流水",
                       "库存流水.csv",
                       "CSV文件 (*.csv);;所有文件 (*.*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件");
        return;
    }

    // 添加UTF-8 BOM标记（Excel识别UTF-8的关键）
    QByteArray bom;
    bom.append(0xEF);
    bom.append(0xBB);
    bom.append(0xBF);
    file.write(bom);

    QTextStream out(&file);

    // 设置UTF-8编码
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif

    // 写入表头
    out << "药品ID,操作类型,数量,操作员,操作时间,备注\n";

    // 写入数据
    for (int i = 0; i < proxyModel->rowCount(); ++i) {
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyModel->index(i, 0));

        QString medicineId = logModel->data(logModel->index(sourceIndex.row(),
                                            logModel->fieldIndex("MEDICINE_ID"))).toString();
        QString operationType = getOperationTypeText(logModel->data(logModel->index(sourceIndex.row(),
                                logModel->fieldIndex("OPERATION_TYPE"))).toString());
        QString quantity = logModel->data(logModel->index(sourceIndex.row(),
                                          logModel->fieldIndex("QUANTITY"))).toString();
        QString operatorName = logModel->data(logModel->index(sourceIndex.row(),
                                              logModel->fieldIndex("OPERATOR"))).toString();
        QString operationTime = logModel->data(logModel->index(sourceIndex.row(),
                                               logModel->fieldIndex("OPERATION_TIME"))).toString();
        QString remark = logModel->data(logModel->index(sourceIndex.row(),
                                        logModel->fieldIndex("REMARK"))).toString();

        // 处理CSV格式
        auto formatCsv = [](const QString &str) -> QString {
            QString result = str;
            if (result.contains(',') || result.contains('"') || result.contains('\n'))
            {
                result.replace("\"", "\"\"");
                return "\"" + result + "\"";
            }
            return result;
        };

        out << formatCsv(medicineId) << ","
            << formatCsv(operationType) << ","
            << formatCsv(quantity) << ","
            << formatCsv(operatorName) << ","
            << formatCsv(operationTime) << ","
            << formatCsv(remark) << "\n";
    }

    file.close();
    QMessageBox::information(this, "成功", QString("数据已导出到：%1").arg(fileName));
}


void StockLogView::on_btClose_clicked()
{
    this->close();
}

void StockLogView::setupTableView()
{
    // 设置表格属性
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSortingEnabled(true);

    // 创建模型
    logModel = new QSqlTableModel(this);

    // 检查库存流水表是否存在
    QSqlQuery query;
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='inventory_log'");

    if (query.next()) {
        // 表存在，设置模型
        logModel->setTable("inventory_log");

        // 设置中文表头
        logModel->setHeaderData(logModel->fieldIndex("MEDICINE_ID"), Qt::Horizontal, tr("药品ID"));
        logModel->setHeaderData(logModel->fieldIndex("OPERATION_TYPE"), Qt::Horizontal, tr("操作类型"));
        logModel->setHeaderData(logModel->fieldIndex("QUANTITY"), Qt::Horizontal, tr("数量"));
        logModel->setHeaderData(logModel->fieldIndex("OPERATOR"), Qt::Horizontal, tr("操作员"));
        logModel->setHeaderData(logModel->fieldIndex("OPERATION_TIME"), Qt::Horizontal, tr("操作时间"));
        logModel->setHeaderData(logModel->fieldIndex("REMARK"), Qt::Horizontal, tr("备注"));

        // 创建代理模型
        proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(logModel);
        ui->tableView->setModel(proxyModel);

        // 设置默认排序（按操作时间倒序）
        proxyModel->sort(logModel->fieldIndex("OPERATION_TIME"), Qt::DescendingOrder);

        // 设置列宽
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->tableView->resizeColumnsToContents();

        // 隐藏LOG_ID列
        ui->tableView->hideColumn(0);
    } else {
        // 表不存在，显示提示信息
        ui->tableView->setEnabled(false);
        QMessageBox::information(this, "提示", "库存流水表不存在，尚未有库存操作记录。");
    }
}

void StockLogView::loadLogData()
{
    if (logModel && logModel->tableName() == "inventory_log") {
        logModel->select();

        // 更新统计信息
        int totalRecords = logModel->rowCount();
        ui->lblSummary->setText(QString("共 %1 条记录").arg(totalRecords));
    }
}

QString StockLogView::getOperationTypeText(const QString &operationType)
{
    if (operationType == "IN") return "入库";
    if (operationType == "OUT") return "出库";
    if (operationType == "ADJUST") return "调整";
    return operationType;
}

