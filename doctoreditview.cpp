#include "doctoreditview.h"
#include "ui_doctoreditview.h"
#include "idatabase.h"
#include <QSqlTableModel>

doctorEditView::doctorEditView(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::doctorEditView)
{
    ui->setupUi(this);

    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().doctorTabModel;
    dataMapper->setModel(IDatabase::getInstance().doctorTabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("DOCTOR_ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbEditEmployeeNo, tabModel->fieldIndex("EMPLOYEENO"));
    dataMapper->addMapping(ui->dbEditDeptID, tabModel->fieldIndex("DEPARTMENT_ID"));

    dataMapper->setCurrentIndex(index);

    ui->dbEditID->setEnabled(false);
}

doctorEditView::~doctorEditView()
{
    delete ui;
}

void doctorEditView::on_btSave_clicked()
{
    IDatabase::getInstance().submitDoctorEdit();

    emit goPreviousView();
}


void doctorEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertDoctorEdit();

    emit goPreviousView();
}

