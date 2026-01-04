#include "visiteditview.h"
#include "ui_visiteditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

VisitEditView::VisitEditView(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::VisitEditView),
    currentIndex(index)
{
    ui->setupUi(this);

    // 初始化数据映射
    dataMapper = new QDataWidgetMapper(this);

    // 设置模型
    QSqlTableModel *tabModel = IDatabase::getInstance().visitTabModel;
    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    // 设置字段映射
    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("VISIT_ID"));
    dataMapper->addMapping(ui->comboDepartment, tabModel->fieldIndex("DEPARTMENT_ID"));
    dataMapper->addMapping(ui->comboDoctor, tabModel->fieldIndex("DOCTOR_ID"));
    dataMapper->addMapping(ui->comboPatient, tabModel->fieldIndex("PATIENT_ID"));
    dataMapper->addMapping(ui->dateEditVisitDate, tabModel->fieldIndex("VISITTIME"));
    dataMapper->addMapping(ui->dbEditSymptoms, tabModel->fieldIndex("SYMPTOMS"));
    dataMapper->addMapping(ui->dbEditDiagnosis, tabModel->fieldIndex("DIAGNOSIS"));
    dataMapper->addMapping(ui->dbEditPrescription, tabModel->fieldIndex("PRESCRIPTION"));
    dataMapper->addMapping(ui->dbEditCreatedTimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));

    // 初始化下拉框
    initComboBoxes();

    // 设置当前索引
    if (index >= 0 && index < tabModel->rowCount()) {
        dataMapper->setCurrentIndex(index);
    }

    ui->dbEditID->setEnabled(false);

    // 连接科室选择信号
    connect(ui->comboDepartment, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VisitEditView::onDoctorSelected);

    // 如果是新增记录，设置默认值
    if (index == tabModel->rowCount() - 1) {
        ui->dateEditVisitDate->setDate(QDate::currentDate());
        ui->dbEditCreatedTimeStamp->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
}

VisitEditView::~VisitEditView()
{
    delete ui;
}

void VisitEditView::on_btSave_clicked()
{
    // 验证数据
    if (ui->comboPatient->currentIndex() < 0 ||
            ui->comboDoctor->currentIndex() < 0 ||
            ui->comboDepartment->currentIndex() < 0) {
        QMessageBox::warning(this, "保存失败", "请选择患者、医生和科室");
        return;
    }

    // 获取当前行和模型
    int row = dataMapper->currentIndex();
    QSqlTableModel *model = IDatabase::getInstance().visitTabModel;

    // 手动设置所有字段
    model->setData(model->index(row, model->fieldIndex("PATIENT_ID")),
                   ui->comboPatient->currentData());
    model->setData(model->index(row, model->fieldIndex("DOCTOR_ID")),
                   ui->comboDoctor->currentData());
    model->setData(model->index(row, model->fieldIndex("DEPARTMENT_ID")),
                   ui->comboDepartment->currentData());
    model->setData(model->index(row, model->fieldIndex("VISITTIME")),
                   ui->dateEditVisitDate->date().toString("yyyy-MM-dd"));
    model->setData(model->index(row, model->fieldIndex("SYMPTOMS")),
                   ui->dbEditSymptoms->text());
    model->setData(model->index(row, model->fieldIndex("DIAGNOSIS")),
                   ui->dbEditDiagnosis->text());
    model->setData(model->index(row, model->fieldIndex("PRESCRIPTION")),
                   ui->dbEditPrescription->text());

    // 如果是新增记录，设置创建日期
    if (row == model->rowCount() - 1) {
        model->setData(model->index(row, model->fieldIndex("CREATEDTIMESTAMP")),
                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }

    // 提交更改
    if (IDatabase::getInstance().submitVisitEdit()) {
        QMessageBox::information(this, "成功", "就诊记录保存成功");
        emit goPreviousView();
    } else {
        QMessageBox::warning(this, "保存失败", "保存就诊记录时发生错误");
    }
}


void VisitEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertVisitEdit();
    emit goPreviousView();
}

void VisitEditView::onDoctorSelected(int index)
{
    if (index >= 0) {
        int departmentId = ui->comboDepartment->itemData(index).toInt();
        loadDoctorByDepartment(departmentId);
    }
}

void VisitEditView::initComboBoxes()
{
    // 清空下拉框
    ui->comboPatient->clear();
    ui->comboDoctor->clear();
    ui->comboDepartment->clear();

    // 加载患者数据
    QSqlQuery queryPatient("SELECT PATIENT_ID, NAME FROM Patient ORDER BY NAME");
    while (queryPatient.next()) {
        int id = queryPatient.value("PATIENT_ID").toInt();
        QString name = queryPatient.value("NAME").toString();
        ui->comboPatient->addItem(name, id);
    }

    // 加载科室数据
    QSqlQuery queryDept("SELECT DEPARTMENT_ID, NAME FROM Department ORDER BY NAME");
    while (queryDept.next()) {
        int id = queryDept.value("DEPARTMENT_ID").toInt();
        QString name = queryDept.value("NAME").toString();
        ui->comboDepartment->addItem(name, id);
    }

    // 默认加载第一个科室的医生
    if (ui->comboDepartment->count() > 0) {
        int firstDeptId = ui->comboDepartment->itemData(0).toInt();
        loadDoctorByDepartment(firstDeptId);
    }
}

void VisitEditView::loadDoctorByDepartment(int departmentId)
{
    ui->comboDoctor->clear();

    QSqlQuery query;
    query.prepare("SELECT Doctor_ID, NAME FROM Doctor WHERE DEPARTMENT_ID = ? ORDER BY NAME");
    query.addBindValue(departmentId);

    if (query.exec()) {
        while (query.next()) {
            int id = query.value("Doctor_ID").toInt();
            QString name = query.value("NAME").toString();
            ui->comboDoctor->addItem(name, id);
        }
    } else {
        qDebug() << "加载医生失败:" << query.lastError().text();
    }
}

