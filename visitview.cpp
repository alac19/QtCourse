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
    }
}

VisitView::~VisitView()
{
    delete ui;
}

void VisitView::on_btSearch_clicked()
{
    QString searchText = ui->txtSearch->text().trimmed();
    if (searchText.isEmpty()) {
        // 如果搜索框为空，显示所有记录
        IDatabase::getInstance().searchVisit("");
        return;
    }

    // 构建查询条件：按患者姓名、医生姓名或症状搜索
    QString filter = QString(
                         "PATIENT_ID IN (SELECT PATIENT_ID FROM Patient WHERE NAME LIKE '%%1%') "
                         "OR DOCTOR_ID IN (SELECT Doctor_ID FROM Doctor WHERE NAME LIKE '%%1%') "
                         "OR SYMPTOMS LIKE '%%1%' "
                         "OR DIAGNOSIS LIKE '%%1%'"
                     ).arg(searchText);

    IDatabase::getInstance().searchVisit(filter);
}


void VisitView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewVisit();
    emit goVisitEditView(currow);
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
    QModelIndex curIndex = IDatabase::getInstance().theVisitSelection->currentIndex();
    if (!curIndex.isValid()) {
        QMessageBox::warning(this, "修改失败", "请先选择一条记录");
        return;
    }

    emit goVisitEditView(curIndex.row());
}

