#include "idatabase.h"
#include <QUuid>
#include <QSqlRelationalTableModel>
#include <QSqlRelation>

void IDatabase::initDatabase()
{
    dataBase = QSqlDatabase::addDatabase("QSQLITE");   // 添加 SQL LITE 数据库驱动
    QString aFile = "D:/DevWorkspace/QtCode/hospital.db";
    dataBase.setDatabaseName(aFile);   // 设置数据库名称

    if (!dataBase.open()) {
        // 打开数据库
        qDebug() << "failed to open dataBase";
    } else {
        qDebug() << "open dataBase is ok";
    }
}

bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this, dataBase);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(
        QSqlTableModel::OnManualSubmit);   // 数据保存方式
    patientTabModel->setSort(patientTabModel->fieldIndex("name"), Qt::AscendingOrder);


    // 设置中文表头
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("PATIENT_ID"), Qt::Horizontal, "ID");
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("NAME"), Qt::Horizontal, "姓名");
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("ID_CARD"), Qt::Horizontal, "身份证号");
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("SEX"), Qt::Horizontal, "性别");
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("AGE"), Qt::Horizontal, "年龄");
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("DOB"), Qt::Horizontal, "出生日期");
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("HEIGHT"), Qt::Horizontal, "身高");
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("WEIGHT"), Qt::Horizontal, "体重");
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("MOBILEPHONE"), Qt::Horizontal, "电话号码");
    patientTabModel->setHeaderData(patientTabModel->fieldIndex("CREATEDTIMESTAMP"), Qt::Horizontal, "创建日期");

    // 查询数据
    if (!(patientTabModel->select())) {
        return false;
    }

    thePatientSelection = new QItemSelectionModel(patientTabModel);

    return true;
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1, 1);

    int curRecNO = curIndex.row();
    QSqlRecord curRec = patientTabModel->record(curRecNO);
    curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("PATIENT_ID", QUuid::createUuid().toString(QUuid::WithoutBraces));

    patientTabModel->setRecord(curRecNO, curRec);

    return curIndex.row();
}

bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    if (!thePatientSelection->hasSelection()) {
        return false;
    }
    int delRow = thePatientSelection->currentIndex().row();
    patientTabModel->removeRow(delRow);
    patientTabModel->submitAll();
    patientTabModel->select();
    // 删除后选中新的行（比如第一行）
    if (patientTabModel->rowCount() > 0) {
        thePatientSelection->setCurrentIndex(patientTabModel->index(0, 0), QItemSelectionModel::Select);
    }
    return true;
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    patientTabModel->revertAll();
}

bool IDatabase::initDoctorModel()
{
    doctorTabModel = new QSqlTableModel(this, dataBase);
    doctorTabModel->setTable("doctor");
    doctorTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    doctorTabModel->setSort(doctorTabModel->fieldIndex("name"), Qt::AscendingOrder);

    // 设置中文表头
    doctorTabModel->setHeaderData(doctorTabModel->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "ID");
    doctorTabModel->setHeaderData(doctorTabModel->fieldIndex("NAME"), Qt::Horizontal, "姓名");
    doctorTabModel->setHeaderData(doctorTabModel->fieldIndex("EMPLOYEENO"), Qt::Horizontal, "工号");
    doctorTabModel->setHeaderData(doctorTabModel->fieldIndex("DEPARTMENT_ID"), Qt::Horizontal, "科室ID");

    if (!(doctorTabModel->select())) {
        return false;
    }

    theDoctorSelection = new QItemSelectionModel(doctorTabModel);
    return true;
}

int IDatabase::addNewDoctor()
{
    doctorTabModel->insertRow(doctorTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = doctorTabModel->index(doctorTabModel->rowCount() - 1, 1);
    int curRecNO = curIndex.row();
    QSqlRecord curRec = doctorTabModel->record(curRecNO);
    curRec.setValue("DOCTOR_ID", QUuid::createUuid().toString(QUuid::WithoutBraces));

    doctorTabModel->setRecord(curRecNO, curRec);

    // 如果有需要，可以设置默认值
    return curIndex.row();
}

bool IDatabase::searchDoctor(QString filter)
{
    doctorTabModel->setFilter(filter);
    return doctorTabModel->select();
}

bool IDatabase::deleteCurrentDoctor()
{
    if (!theDoctorSelection->hasSelection()) {
        return false;
    }

    int delRow = theDoctorSelection->currentIndex().row();
    doctorTabModel->removeRow(delRow);
    doctorTabModel->submitAll();
    doctorTabModel->select();

    if (doctorTabModel->rowCount() > 0) {
        theDoctorSelection->setCurrentIndex(doctorTabModel->index(0, 0), QItemSelectionModel::Select);
    }

    return true;
}

bool IDatabase::submitDoctorEdit()
{
    return doctorTabModel->submitAll();
}

void IDatabase::revertDoctorEdit()
{
    doctorTabModel->revertAll();
}

bool IDatabase::initDepartmentModel()
{
    departmentTabModel = new QSqlTableModel(this, dataBase);
    departmentTabModel->setTable("department");
    departmentTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    departmentTabModel->setSort(departmentTabModel->fieldIndex("name"), Qt::AscendingOrder);

    // 设置中文表头
    departmentTabModel->setHeaderData(departmentTabModel->fieldIndex("DEPARTMENT_ID"), Qt::Horizontal, "ID");
    departmentTabModel->setHeaderData(departmentTabModel->fieldIndex("NAME"), Qt::Horizontal, "名称");

    if (!(departmentTabModel->select())) {
        return false;
    }

    theDepartmentSelection = new QItemSelectionModel(departmentTabModel);
    return true;
}

int IDatabase::addNewDepartment()
{
    departmentTabModel->insertRow(departmentTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = departmentTabModel->index(departmentTabModel->rowCount() - 1, 1);
    int curRecNO = curIndex.row();
    QSqlRecord curRec = departmentTabModel->record(curRecNO);
    curRec.setValue("DEPARTMENT_ID", QUuid::createUuid().toString(QUuid::WithoutBraces));

    departmentTabModel->setRecord(curRecNO, curRec);

    // 如果有需要，可以设置默认值
    return curIndex.row();
}

bool IDatabase::searchDepartment(QString filter)
{
    departmentTabModel->setFilter(filter);
    return departmentTabModel->select();
}

bool IDatabase::deleteCurrentDepartment()
{
    if (!theDepartmentSelection->hasSelection()) {
        return false;
    }

    int delRow = theDepartmentSelection->currentIndex().row();
    departmentTabModel->removeRow(delRow);
    departmentTabModel->submitAll();
    departmentTabModel->select();

    if (departmentTabModel->rowCount() > 0) {
        theDepartmentSelection->setCurrentIndex(departmentTabModel->index(0, 0), QItemSelectionModel::Select);
    }

    return true;
}

bool IDatabase::submitDepartmentEdit()
{
    return departmentTabModel->submitAll();
}

void IDatabase::revertDepartmentEdit()
{
    departmentTabModel->revertAll();
}

bool IDatabase::initVisitModel()
{
    // 改为使用关系表模型
    visitTabModel = new QSqlRelationalTableModel(this, dataBase);
    visitTabModel->setTable("Visit_Record");
    visitTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    visitTabModel->setSort(visitTabModel->fieldIndex("VISITTIME"), Qt::AscendingOrder);

    // 设置关系：将ID字段关联到其他表的名称字段
    // 患者ID -> 患者姓名
    visitTabModel->setRelation(visitTabModel->fieldIndex("PATIENT_ID"),
                               QSqlRelation("Patient", "PATIENT_ID", "NAME"));

    // 医生ID -> 医生姓名
    visitTabModel->setRelation(visitTabModel->fieldIndex("DOCTOR_ID"),
                               QSqlRelation("Doctor", "Doctor_ID", "NAME"));

    // 科室ID -> 科室名称
    visitTabModel->setRelation(visitTabModel->fieldIndex("DEPARTMENT_ID"),
                               QSqlRelation("Department", "DEPARTMENT_ID", "NAME"));

    // 设置中文表头
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("VISIT_ID"), Qt::Horizontal, "记录ID");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("PATIENT_ID"), Qt::Horizontal, "患者ID");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "医生ID");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("DEPARTMENT_ID"), Qt::Horizontal, "科室ID");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("VISITTIME"), Qt::Horizontal, "就诊日期");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("SYMPTOMS"), Qt::Horizontal, "症状");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("DIAGNOSIS"), Qt::Horizontal, "诊断");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("PRESCRIPTION"), Qt::Horizontal, "处方");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("CREATEDTIMESTAMP"), Qt::Horizontal, "创建时间");

    if (!visitTabModel->select()) {
        return false;
    }

    theVisitSelection = new QItemSelectionModel(visitTabModel);
    return true;
}

int IDatabase::addNewVisit()
{
    visitTabModel->insertRow(visitTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = visitTabModel->index(visitTabModel->rowCount() - 1, 1);

    int curRecNO = curIndex.row();
    QSqlRecord curRec = visitTabModel->record(curRecNO);
    curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("Visit_ID", QUuid::createUuid().toString(QUuid::WithoutBraces));

    visitTabModel->setRecord(curRecNO, curRec);

    return curIndex.row();
}

bool IDatabase::searchVisit(QString filter)
{
    visitTabModel->setFilter(filter);
    return visitTabModel->select();
}

bool IDatabase::deleteCurrentVisit()
{
    {
        if (!theVisitSelection->hasSelection()) {
            return false;
        }

        int delRow = theVisitSelection->currentIndex().row();
        visitTabModel->removeRow(delRow);
        visitTabModel->submitAll();
        visitTabModel->select();

        if (visitTabModel->rowCount() > 0) {
            theVisitSelection->setCurrentIndex(visitTabModel->index(0, 0), QItemSelectionModel::Select);
        }

        return true;
    }
}

bool IDatabase::submitVisitEdit()
{
    return visitTabModel->submitAll();
}

void IDatabase::revertVisitEdit()
{
    visitTabModel->revertAll();
}

QString IDatabase::userLogin(QString userName, QString passWord)
{
    QSqlQuery query;
    query.prepare("select username, password from user where username = :USER");
    query.bindValue(":USER", userName);
    query.exec();

    if (query.first() && query.value("username").isValid()) {
        QString password = query.value("password").toString();

        if (password == passWord) {
            qDebug() << "login ok";
            return "loginOk";
        } else {
            qDebug() << "wrong password";
            return "wrongPassword";
        }
    } else {
        qDebug() << "no such user";
        return "wrongUsername";
    }
}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    initDatabase();
}
