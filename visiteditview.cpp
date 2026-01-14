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

    // 设置日期显示格式
    ui->dateEditVisitDate->setDisplayFormat("yyyy-MM-dd");
    ui->dateEditCreatedTimeStamp->setDisplayFormat("yyyy-MM-dd");

    // 初始化下拉框
    initComboBoxes();

    // 获取模型
    QSqlTableModel *tabModel = IDatabase::getInstance().visitTabModel;

    // 先设置默认日期为当前日期
    QDate currentDate = QDate::currentDate();
    ui->dateEditCreatedTimeStamp->setDate(currentDate);

    if (index >= 0 && index < tabModel->rowCount()) {
        // 加载现有记录
        loadRecord(index);
    } else {
        // 新增记录，设置默认值
        ui->dateEditVisitDate->setDate(currentDate);

        // 设置默认选中
        if (ui->comboPatient->count() > 0) ui->comboPatient->setCurrentIndex(0);
        if (ui->comboDepartment->count() > 0) {
            ui->comboDepartment->setCurrentIndex(0);
            // 改为字符串
            QString firstDeptId = ui->comboDepartment->itemData(0).toString();
            loadDoctorByDepartment(firstDeptId);
            if (ui->comboDoctor->count() > 0) ui->comboDoctor->setCurrentIndex(0);
        }
    }

    // 连接科室选择信号
    connect(ui->comboDepartment, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onDepartmentChanged(int)));
}

VisitEditView::~VisitEditView()
{
    delete ui;
}

void VisitEditView::loadRecord(int row)
{
    QSqlTableModel *tabModel = IDatabase::getInstance().visitTabModel;

    // 获取当前行数据
    QSqlRecord record = tabModel->record(row);

    // 设置基本字段
    ui->dbEditID->setText(record.value("VISIT_ID").toString());

    // 就诊时间
    QDate visitDate = record.value("VISITTIME").toDate();
    if (visitDate.isValid()) {
        ui->dateEditVisitDate->setDate(visitDate);
    } else {
        ui->dateEditVisitDate->setDate(QDate::currentDate());
    }

    ui->dbEditSymptoms->setText(record.value("SYMPTOMS").toString());
    ui->dbEditDiagnosis->setText(record.value("DIAGNOSIS").toString());
    ui->dbEditPrescription->setText(record.value("PRESCRIPTION").toString());

    // 创建时间
    QDate createDate = record.value("CREATEDTIMESTAMP").toDate();
    if (createDate.isValid()) {
        ui->dateEditCreatedTimeStamp->setDate(createDate);
    } else {
        ui->dateEditCreatedTimeStamp->setDate(QDate::currentDate());
    }

    // 设置下拉框选中项 - 改为字符串
    QString patientId = record.value("PATIENT_ID").toString();
    QString deptId = record.value("DEPARTMENT_ID").toString();

    // 设置患者
    int patientIndex = ui->comboPatient->findData(patientId);
    if (patientIndex >= 0) {
        ui->comboPatient->setCurrentIndex(patientIndex);
    } else {
        ui->comboPatient->setCurrentIndex(0); // 默认选择第一个
    }

    // 设置科室
    int deptIndex = ui->comboDepartment->findData(deptId);
    if (deptIndex >= 0) {
        ui->comboDepartment->setCurrentIndex(deptIndex);

        // 加载该科室的医生 - 使用字符串ID
        loadDoctorByDepartment(deptId);

        // 设置医生
        QString doctorId = record.value("DOCTOR_ID").toString();
        int doctorIndex = ui->comboDoctor->findData(doctorId);
        if (doctorIndex >= 0) {
            ui->comboDoctor->setCurrentIndex(doctorIndex);
        } else {
            if (ui->comboDoctor->count() > 0) ui->comboDoctor->setCurrentIndex(0);
        }
    } else {
        if (ui->comboDepartment->count() > 0) {
            ui->comboDepartment->setCurrentIndex(0);
            // 改为字符串
            QString firstDeptId = ui->comboDepartment->itemData(0).toString();
            loadDoctorByDepartment(firstDeptId);
            if (ui->comboDoctor->count() > 0) ui->comboDoctor->setCurrentIndex(0);
        }
    }
}


void VisitEditView::on_btSave_clicked()
{
    // 验证数据
    if (ui->comboDepartment->currentIndex() < 0) {
        QMessageBox::warning(this, "保存失败", "请选择科室");
        return;
    }

    if (ui->comboDoctor->currentIndex() < 0) {
        QMessageBox::warning(this, "保存失败", "请选择医生");
        return;
    }

    if (ui->comboPatient->currentIndex() < 0) {
        QMessageBox::warning(this, "保存失败", "请选择患者");
        return;
    }

    // 获取当前模型
    QSqlTableModel *model = IDatabase::getInstance().visitTabModel;

    // 判断是新增还是修改
    int row = currentIndex;

    // 获取选中的ID - 改为字符串
    QString patientId = ui->comboPatient->currentData().toString();
    QString doctorId = ui->comboDoctor->currentData().toString();
    QString departmentId = ui->comboDepartment->currentData().toString();

    qDebug() << "选中的ID - 患者:" << patientId << "医生:" << doctorId << "科室:" << departmentId;

    // 验证ID是否在数据库中真实存在 - 使用字符串查询
    QSqlQuery checkQuery;

    // 检查患者是否存在
    checkQuery.prepare("SELECT COUNT(*) FROM Patient WHERE PATIENT_ID = ?");
    checkQuery.addBindValue(patientId); // 传递字符串
    if (checkQuery.exec() && checkQuery.next()) {
        if (checkQuery.value(0).toInt() == 0) {
            QMessageBox::warning(this, "保存失败", "选择的患者不存在");
            return;
        }
    }

    // 检查医生是否存在
    checkQuery.prepare("SELECT COUNT(*) FROM Doctor WHERE Doctor_ID = ?");
    checkQuery.addBindValue(doctorId); // 传递字符串
    if (checkQuery.exec() && checkQuery.next()) {
        if (checkQuery.value(0).toInt() == 0) {
            QMessageBox::warning(this, "保存失败", "选择的医生不存在");
            return;
        }
    }

    // 检查科室是否存在
    checkQuery.prepare("SELECT COUNT(*) FROM Department WHERE DEPARTMENT_ID = ?");
    checkQuery.addBindValue(departmentId); // 传递字符串
    if (checkQuery.exec() && checkQuery.next()) {
        if (checkQuery.value(0).toInt() == 0) {
            QMessageBox::warning(this, "保存失败", "选择的科室不存在");
            return;
        }
    }

    // 获取日期
    QDate visitDate = ui->dateEditVisitDate->date();
    QDate createDate = ui->dateEditCreatedTimeStamp->date();

    qDebug() << "就诊日期:" << visitDate.toString("yyyy-MM-dd");
    qDebug() << "创建日期:" << createDate.toString("yyyy-MM-dd");

    // 手动设置所有字段到模型 - 使用字符串ID
    model->setData(model->index(row, model->fieldIndex("PATIENT_ID")), patientId);
    model->setData(model->index(row, model->fieldIndex("DOCTOR_ID")), doctorId);
    model->setData(model->index(row, model->fieldIndex("DEPARTMENT_ID")), departmentId);
    model->setData(model->index(row, model->fieldIndex("VISITTIME")),
                   visitDate.toString("yyyy-MM-dd"));
    model->setData(model->index(row, model->fieldIndex("SYMPTOMS")),
                   ui->dbEditSymptoms->text());
    model->setData(model->index(row, model->fieldIndex("DIAGNOSIS")),
                   ui->dbEditDiagnosis->text());
    model->setData(model->index(row, model->fieldIndex("PRESCRIPTION")),
                   ui->dbEditPrescription->text());
    model->setData(model->index(row, model->fieldIndex("CREATEDTIMESTAMP")),
                   createDate.toString("yyyy-MM-dd"));

    // 调试：打印所有设置的值
    for (int i = 0; i < model->columnCount(); ++i) {
        QString fieldName = model->headerData(i, Qt::Horizontal).toString();
        QVariant value = model->data(model->index(row, i));
        qDebug() << fieldName << ":" << value.toString();
    }

    // 提交到数据库
    if (IDatabase::getInstance().submitVisitEdit()) {
        qDebug() << "提交成功";
        QMessageBox::information(this, "成功", "就诊记录保存成功");
        emit goPreviousView();
    } else {
        QString error = model->lastError().text();
        qDebug() << "提交失败:" << error;
        QMessageBox::warning(this, "保存失败", "保存就诊记录时发生错误:\n" + error);
    }
}


void VisitEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertVisitEdit();
    emit goPreviousView();
}

void VisitEditView::onDepartmentChanged(int index)
{
    if (index >= 0) {
        // 改为字符串
        QString departmentId = ui->comboDepartment->itemData(index).toString();
        loadDoctorByDepartment(departmentId);
    }
}

void VisitEditView::initComboBoxes()
{
    // 清空下拉框
    ui->comboPatient->clear();
    ui->comboDoctor->clear();
    ui->comboDepartment->clear();

    // 加载患者数据 - ID是字符串
    QSqlQuery queryPatient("SELECT PATIENT_ID, NAME FROM Patient ORDER BY NAME");
    while (queryPatient.next()) {
        // 改为字符串
        QString id = queryPatient.value("PATIENT_ID").toString();
        QString name = queryPatient.value("NAME").toString();
        ui->comboPatient->addItem(name, id);
    }

    // 加载科室数据 - ID是字符串
    QSqlQuery queryDept("SELECT DEPARTMENT_ID, NAME FROM Department ORDER BY NAME");
    while (queryDept.next()) {
        // 改为字符串
        QString id = queryDept.value("DEPARTMENT_ID").toString();
        QString name = queryDept.value("NAME").toString();
        ui->comboDepartment->addItem(name, id);
    }

    // 默认加载第一个科室的医生
    if (ui->comboDepartment->count() > 0) {
        // 改为字符串
        QString firstDeptId = ui->comboDepartment->itemData(0).toString();
        loadDoctorByDepartment(firstDeptId);
    }
}

void VisitEditView::loadDoctorByDepartment(const QString &departmentId)
{
    ui->comboDoctor->clear();

    QSqlQuery query;
    query.prepare("SELECT Doctor_ID, NAME FROM Doctor WHERE DEPARTMENT_ID = ? ORDER BY NAME");
    query.addBindValue(departmentId); // 传递字符串

    if (query.exec()) {
        while (query.next()) {
            // 改为字符串
            QString id = query.value("Doctor_ID").toString();
            QString name = query.value("NAME").toString();
            ui->comboDoctor->addItem(name, id);
        }
    } else {
        qDebug() << "加载医生失败:" << query.lastError().text();
    }
}
