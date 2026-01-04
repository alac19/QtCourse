#include "departmentview.h"
#include "ui_departmentview.h"
#include "idatabase.h"

DepartmentView::DepartmentView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DepartmentView)
{
    ui->setupUi(this);

    // 设置表格属性
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    // 初始化医生模型
    IDatabase &iDatabase = IDatabase::getInstance();
    if(iDatabase.initDepartmentModel()) {
        ui->tableView->setModel(iDatabase.departmentTabModel);
        ui->tableView->setSelectionModel(iDatabase.theDepartmentSelection);
    }
}

DepartmentView::~DepartmentView()
{
    delete ui;
}

void DepartmentView::on_btSearch_clicked()
{
    QString filter = QString("NAME like '%%1%'").arg(ui->txtSearch->text());
    IDatabase::getInstance().searchDepartment(filter);
}


void DepartmentView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewDepartment();
    emit goDepartmentEditView(currow);
}


void DepartmentView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentDepartment();
}


void DepartmentView::on_btModify_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().theDepartmentSelection->currentIndex();
    if (curIndex.isValid()) {
        emit goDepartmentEditView(curIndex.row());
    }
}

