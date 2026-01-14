#include "visitview.h"
#include "ui_visitview.h"
#include "idatabase.h"
#include <QMessageBox>

VisitView::VisitView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisitView)
{
    ui->setupUi(this);

    // 设置表格属性
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    // 初始化数据库模型
    IDatabase &iDatabase = IDatabase::getInstance();
    if(iDatabase.initVisitModel()) {
        ui->tableView->setModel(iDatabase.visitTabModel);
        ui->tableView->setSelectionModel(iDatabase.theVisitSelection);

        // 直接隐藏前4列（假设顺序是：VISIT_ID, DEPARTMENT_ID, DOCTOR_ID, PATIENT_ID）
        // ui->tableView->hideColumn(0); // VISIT_ID
        ui->tableView->hideColumn(1); // DEPARTMENT_ID
        ui->tableView->hideColumn(2); // DOCTOR_ID
        ui->tableView->hideColumn(3); // PATIENT_ID
    }
}

VisitView::~VisitView()
{
    delete ui;
}

void VisitView::on_btSearch_clicked()
{
    QString searchText = ui->txtSearch->text();
    if (searchText.isEmpty()) {
        // 清空筛选，显示所有记录
        IDatabase::getInstance().visitTabModel->setFilter("");
    } else {
        // 可以按患者姓名、医生姓名、科室名称搜索
        QString filter = QString(
                             "PATIENT_NAME LIKE '%%1%' OR DOCTOR_NAME LIKE '%%1%' OR DEPARTMENT_NAME LIKE '%%1%'"
                         ).arg(searchText);
        IDatabase::getInstance().searchVisit(filter);
    }
}


void VisitView::on_btAdd_clicked()
{
    qDebug() << "=== 开始添加就诊记录 ===";

    try {
        int currow = IDatabase::getInstance().addNewVisit();
        qDebug() << "添加新记录成功，行号:" << currow;

        if (currow >= 0) {
            emit goVisitEditView(currow);
            qDebug() << "已发出跳转信号";
        } else {
            QMessageBox::warning(this, "错误", "添加新记录失败");
        }
    } catch (const std::exception& e) {
        qDebug() << "异常:" << e.what();
        QMessageBox::critical(this, "错误", QString("添加记录时发生异常: %1").arg(e.what()));
    } catch (...) {
        qDebug() << "未知异常";
        QMessageBox::critical(this, "错误", "添加记录时发生未知异常");
    }

    qDebug() << "=== 添加就诊记录结束 ===";
}


void VisitView::on_btDelete_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                  "确定要删除选中的就诊记录吗？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (!IDatabase::getInstance().deleteCurrentVisit()) {
            QMessageBox::warning(this, "删除失败", "请先选择一条记录");
        }
    }
}


void VisitView::on_btModify_clicked()
{
    if (!IDatabase::getInstance().theVisitSelection->hasSelection()) {
        QMessageBox::warning(this, "提示", "请先选择一条记录");
        return;
    }

    QModelIndex curIndex = IDatabase::getInstance().theVisitSelection->currentIndex();
    emit goVisitEditView(curIndex.row());
}

