#include "patienteditview.h"
#include "ui_patienteditview.h"
#include "idatabase.h"
#include <QSqlTableModel>

PatientEditView::PatientEditView(QWidget *parent, int index)
    : QWidget(parent)
    , ui(new Ui::PatientEditView)
{
    ui->setupUi(this);

    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().patientTabModel;
    dataMapper->setModel(IDatabase::getInstance().patientTabModel);
    // 改为手动提交策略
    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("PATIENT_ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbEditIDCard, tabModel->fieldIndex("ID_CARD"));
    dataMapper->addMapping(ui->dbEditSex, tabModel->fieldIndex("SEX"));
    dataMapper->addMapping(ui->dbEditAge, tabModel->fieldIndex("AGE"));
    dataMapper->addMapping(ui->dateEditDOB, tabModel->fieldIndex("DOB"));
    dataMapper->addMapping(ui->dbEditHeight, tabModel->fieldIndex("HEIGHT"));
    dataMapper->addMapping(ui->dbEditWeight, tabModel->fieldIndex("WEIGHT"));
    dataMapper->addMapping(ui->dbEditMobile, tabModel->fieldIndex("MOBILEPHONE"));
    dataMapper->addMapping(ui->dateEditCreatedTimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));

    dataMapper->setCurrentIndex(index);

    // // 对于新增记录，手动设置默认值
    // if (index >= tabModel->rowCount()) {
    //     // 新增记录，设置默认日期
    //     ui->dateEditCreatedTimeStamp->setDate(QDate::currentDate());
    //     ui->dateEditDOB->setDate(QDate(2000, 1, 1));
    // }

    // 重要：对于新增记录，手动设置日期
    if (index >= 0 && index < tabModel->rowCount()) {
        // 修改记录：检查是否有有效的日期，如果没有则设置默认值
        QDate createDate = tabModel->record(index).value("CREATEDTIMESTAMP").toDate();
        if (!createDate.isValid()) {
            ui->dateEditCreatedTimeStamp->setDate(QDate::currentDate());
        }

        QDate dob = tabModel->record(index).value("DOB").toDate();
        if (!dob.isValid()) {
            ui->dateEditDOB->setDate(QDate(2000, 1, 1));
        }
    }

    // ui->dbEditID->setEnabled(false);
}

PatientEditView::~PatientEditView()
{
    delete ui;
}

void PatientEditView::on_btSave_clicked()
{
    // 手动提交QDataWidgetMapper的更改
    dataMapper->submit();

    // 再提交到数据库
    IDatabase::getInstance().submitPatientEdit();

    emit goPreviousView();
}


void PatientEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertPatientEdit();

    emit goPreviousView();
}

