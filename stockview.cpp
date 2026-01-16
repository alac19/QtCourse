#include "stockview.h"
#include "ui_stockview.h"
#include "stockoperationdialog.h"
#include "stockadjustdialog.h"
// #include "medicinedetailview.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QUuid>

StockView::StockView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StockView),
    inventoryModel(nullptr),
    proxyModel(nullptr)
{
    ui->setupUi(this);

    // 初始化界面
    setupTableView();

    // 加载库存数据
    loadInventoryData();

    // 更新摘要信息
    updateSummary();

    // 连接信号槽
    connect(ui->tableView, &QTableView::doubleClicked,
            this, &StockView::on_tableView_doubleClicked);
}

StockView::~StockView()
{
    delete ui;
    delete proxyModel;
}

void StockView::on_btSearch_clicked()
{
    QString keyword = ui->txtSearch->text().trimmed();

    if (keyword.isEmpty()) {
        // 清空过滤
        proxyModel->setFilterFixedString("");
    } else {
        // 构建过滤条件（搜索药品名称、分类、规格、制造商）
        QString filter = QString("NAME LIKE '%%1%' OR CATEGORY LIKE '%%1%' "
                                 "OR SPECIFICATION LIKE '%%1%' OR MANUFACTURER LIKE '%%1%'")
                         .arg(keyword);
        proxyModel->setFilterKeyColumn(-1); // 在所有列中搜索
        proxyModel->setFilterFixedString(keyword);
    }

    updateSummary();
}


void StockView::on_btRefresh_clicked()
{
    loadInventoryData();
    updateSummary();
    QMessageBox::information(this, "刷新", "库存数据已刷新");
}


void StockView::on_btStockIn_clicked()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();

    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要入库的药品");
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(selectedIndexes.first());
    QString medicineId = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                         inventoryModel->fieldIndex("MEDICINE_ID"))).toString();
    QString medicineName = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                           inventoryModel->fieldIndex("NAME"))).toString();

    // 打开入库对话框
    StockOperationDialog dialog(this, medicineId, medicineName, "IN");
    if (dialog.exec() == QDialog::Accepted) {
        int quantity = dialog.getQuantity();
        QString remark = dialog.getRemark();

        if (performStockOperation(medicineId, quantity, "IN", remark)) {
            QMessageBox::information(this, "成功", QString("药品【%1】入库成功，数量：%2").arg(medicineName).arg(quantity));
            loadInventoryData();
            updateSummary();
        } else {
            QMessageBox::warning(this, "错误", "入库操作失败");
        }
    }
}


void StockView::on_btStockOut_clicked()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();

    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要出库的药品");
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(selectedIndexes.first());
    QString medicineId = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                         inventoryModel->fieldIndex("MEDICINE_ID"))).toString();
    QString medicineName = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                           inventoryModel->fieldIndex("NAME"))).toString();
    int currentStock = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                                            inventoryModel->fieldIndex("STOCK_QUANTITY"))).toInt();

    // 打开出库对话框
    StockOperationDialog dialog(this, medicineId, medicineName, "OUT");
    if (dialog.exec() == QDialog::Accepted) {
        int quantity = dialog.getQuantity();
        QString remark = dialog.getRemark();

        // 检查库存是否充足
        if (quantity > currentStock) {
            QMessageBox::warning(this, "警告",
                                 QString("库存不足！当前库存：%1，出库数量：%2").arg(currentStock).arg(quantity));
            return;
        }

        if (performStockOperation(medicineId, -quantity, "OUT", remark)) {
            QMessageBox::information(this, "成功",
                                     QString("药品【%1】出库成功，数量：%2").arg(medicineName).arg(quantity));
            loadInventoryData();
            updateSummary();
        } else {
            QMessageBox::warning(this, "错误", "出库操作失败");
        }
    }
}


void StockView::on_btAdjust_clicked()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();

    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要调整库存的药品");
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(selectedIndexes.first());
    QString medicineId = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                         inventoryModel->fieldIndex("MEDICINE_ID"))).toString();
    QString medicineName = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                           inventoryModel->fieldIndex("NAME"))).toString();
    int currentStock = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                                            inventoryModel->fieldIndex("STOCK_QUANTITY"))).toInt();

    // 打开调整库存对话框
    StockAdjustDialog dialog(this, medicineId, medicineName, currentStock);
    if (dialog.exec() == QDialog::Accepted) {
        int newStock = dialog.getNewStock();
        QString remark = dialog.getRemark();

        // 计算调整量
        int adjustment = newStock - currentStock;

        if (adjustment != 0) {
            // 使用 "ADJUST" 作为操作类型
            if (performStockOperation(medicineId, adjustment, "ADJUST",
                                      remark.isEmpty() ?
                                      QString("库存调整：从 %1 调整为 %2").arg(currentStock).arg(newStock) :
                                      remark)) {

                QMessageBox::information(this, "成功",
                                         QString("药品【%1】库存调整成功，新库存：%2").arg(medicineName).arg(newStock));
                loadInventoryData();
                updateSummary();
            } else {
                QMessageBox::warning(this, "错误", "库存调整失败");
            }
        } else {
            QMessageBox::information(this, "提示", "库存未发生变化");
        }
    }
}


void StockView::on_btViewLog_clicked()
{
    // 打开库存流水查看界面
    // 这里可以创建一个新的对话框或界面来显示库存操作历史
    QMessageBox::information(this, "功能提示", "库存流水查看功能正在开发中...");
}

void StockView::on_cbFilterType_currentIndexChanged(int index)
{

}


void StockView::on_tableView_doubleClicked(const QModelIndex &index)
{

}

void StockView::setupTableView()
{
    // 设置表格属性
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSortingEnabled(true);

    // 创建模型
    inventoryModel = new QSqlTableModel(this);
    inventoryModel->setTable("medicine");

    // 设置列标题（中文）
    inventoryModel->setHeaderData(inventoryModel->fieldIndex("NAME"), Qt::Horizontal, tr("药品名称"));
    inventoryModel->setHeaderData(inventoryModel->fieldIndex("CATEGORY"), Qt::Horizontal, tr("分类"));
    inventoryModel->setHeaderData(inventoryModel->fieldIndex("SPECIFICATION"), Qt::Horizontal, tr("规格"));
    inventoryModel->setHeaderData(inventoryModel->fieldIndex("UNIT"), Qt::Horizontal, tr("单位"));
    inventoryModel->setHeaderData(inventoryModel->fieldIndex("PRICE"), Qt::Horizontal, tr("价格"));
    inventoryModel->setHeaderData(inventoryModel->fieldIndex("STOCK_QUANTITY"), Qt::Horizontal, tr("当前库存"));
    inventoryModel->setHeaderData(inventoryModel->fieldIndex("MIN_STOCK"), Qt::Horizontal, tr("最低库存"));
    inventoryModel->setHeaderData(inventoryModel->fieldIndex("MANUFACTURER"), Qt::Horizontal, tr("制造商"));

    // 创建代理模型以支持排序和过滤
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(inventoryModel);
    ui->tableView->setModel(proxyModel);

    // 隐藏不需要的列
    ui->tableView->hideColumn(0);  // MEDICINE_ID
    ui->tableView->hideColumn(9);  // PRODUCTION_DATE (如果存在)
    ui->tableView->hideColumn(10); // EXPIRY_DATE (如果存在)
    ui->tableView->hideColumn(11); // IS_PRESCRIPTION (如果存在)
    ui->tableView->hideColumn(12); // CREATEDTIMESTAMP (如果存在)
    ui->tableView->hideColumn(13); // UPDATEDTIMESTAMP (如果存在)

    // 设置列宽
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->resizeColumnsToContents();
}

void StockView::loadInventoryData()
{
    if (inventoryModel) {
        inventoryModel->select();

        // 设置默认排序（按药品名称排序）
        proxyModel->sort(inventoryModel->fieldIndex("NAME"), Qt::AscendingOrder);
    }
}

void StockView::updateSummary()
{
    int totalItems = proxyModel->rowCount();
    int lowStockCount = 0;
    int outOfStockCount = 0;

    // 统计库存状态
    for (int i = 0; i < proxyModel->rowCount(); ++i) {
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyModel->index(i, 0));
        int stock = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                                         inventoryModel->fieldIndex("STOCK_QUANTITY"))).toInt();
        int minStock = inventoryModel->data(inventoryModel->index(sourceIndex.row(),
                                            inventoryModel->fieldIndex("MIN_STOCK"))).toInt();

        if (stock == 0) {
            outOfStockCount++;
        } else if (stock < minStock) {
            lowStockCount++;
        }
    }

    // 更新摘要标签
    QString summaryText = QString("总计：%1 种药品 | 缺货：%2 | 低库存：%3")
                          .arg(totalItems)
                          .arg(outOfStockCount)
                          .arg(lowStockCount);

    ui->lblSummary->setText(summaryText);

    // 设置不同库存状态的文本颜色
    QString styleSheet;
    if (outOfStockCount > 0) {
        styleSheet = "QLabel { color: red; font-weight: bold; }";
    } else if (lowStockCount > 0) {
        styleSheet = "QLabel { color: orange; font-weight: bold; }";
    } else {
        styleSheet = "QLabel { color: green; }";
    }
    ui->lblSummary->setStyleSheet(styleSheet);
}

bool StockView::performStockOperation(const QString &medicineId, int quantity, const QString &operationType, const QString &remark)
{
    QSqlQuery query;

    // 获取数据库连接（从模型获取）
    QSqlDatabase db = inventoryModel->database();

    if (!db.isOpen()) {
        qDebug() << "数据库未连接";
        QMessageBox::warning(this, "错误", "数据库连接失败");
        return false;
    }

    // 开始事务
    if (!db.transaction()) {
        qDebug() << "事务启动失败";
        QMessageBox::warning(this, "错误", "事务启动失败");
        return false;
    }

    try {
        // 1. 更新药品库存
        query.prepare("UPDATE medicine SET STOCK_QUANTITY = STOCK_QUANTITY + ?, "
                      "UPDATEDTIMESTAMP = ? WHERE MEDICINE_ID = ?");
        query.addBindValue(quantity);
        query.addBindValue(QDateTime::currentDateTime());
        query.addBindValue(medicineId);

        if (!query.exec()) {
            qDebug() << "更新库存失败:" << query.lastError().text();
            throw std::runtime_error("更新库存失败");
        }

        // 检查是否更新成功
        if (query.numRowsAffected() <= 0) {
            throw std::runtime_error("未找到药品记录");
        }

        // 2. 创建库存流水表（如果不存在）
        QString createTableSQL =
            "CREATE TABLE IF NOT EXISTS inventory_log ("
            "LOG_ID TEXT PRIMARY KEY, "
            "MEDICINE_ID TEXT NOT NULL, "
            "OPERATION_TYPE TEXT NOT NULL, "
            "QUANTITY INTEGER NOT NULL, "
            "OPERATOR TEXT, "
            "OPERATION_TIME DATETIME NOT NULL, "
            "REMARK TEXT"
            ")";

        if (!query.exec(createTableSQL)) {
            qDebug() << "创建库存流水表失败:" << query.lastError().text();
            // 即使没有流水表，库存更新也应该继续
        }

        // 3. 记录库存流水
        query.prepare("INSERT INTO inventory_log (LOG_ID, MEDICINE_ID, OPERATION_TYPE, "
                      "QUANTITY, OPERATOR, OPERATION_TIME, REMARK) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?)");

        QString logId = QUuid::createUuid().toString();
        query.addBindValue(logId);
        query.addBindValue(medicineId);
        query.addBindValue(operationType);
        query.addBindValue(quantity);

        QString operatorName = "admin"; // 暂时使用固定值
        query.addBindValue(operatorName);

        query.addBindValue(QDateTime::currentDateTime());

        QString remarkText = remark.isEmpty() ?
                             QString("%1操作，数量：%2").arg(operationType == "IN" ? "入库" : "出库").arg(abs(quantity)) :
                             remark;
        query.addBindValue(remarkText);

        if (!query.exec()) {
            qDebug() << "记录流水失败:" << query.lastError().text();
            // 即使流水记录失败，库存更新也应该继续
        }

        // 提交事务
        if (!db.commit()) {
            qDebug() << "事务提交失败";
            throw std::runtime_error("事务提交失败");
        }

        qDebug() << "库存操作成功:" << operationType << "数量:" << quantity;
        return true;

    } catch (const std::exception &e) {
        // 回滚事务
        db.rollback();
        qDebug() << "库存操作失败:" << e.what();
        QMessageBox::warning(this, "错误", QString("库存操作失败：%1").arg(e.what()));
        return false;
    }
}

