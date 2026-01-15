#include "idatabase.h"
#include <QUuid>

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
    patientTabModel->setSort(patientTabModel->fieldIndex("CREATEDTIMESTAMP"), Qt::AscendingOrder);


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
    curRec.setValue("PATIENT_ID", QUuid::createUuid().toString(QUuid::WithoutBraces));
    // curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));

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
    doctorTabModel->setSort(doctorTabModel->fieldIndex("EMPLOYEENO"), Qt::AscendingOrder);

    // 设置中文表头
    doctorTabModel->setHeaderData(doctorTabModel->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "ID");
    doctorTabModel->setHeaderData(doctorTabModel->fieldIndex("NAME"), Qt::Horizontal, "姓名");
    doctorTabModel->setHeaderData(doctorTabModel->fieldIndex("EMPLOYEENO"), Qt::Horizontal, "工号");
    // doctorTabModel->setHeaderData(doctorTabModel->fieldIndex("DEPARTMENT_ID"), Qt::Horizontal, "科室ID");
    doctorTabModel->setHeaderData(doctorTabModel->fieldIndex("DEPARTMENT_NAME"), Qt::Horizontal, "所属科室"); // 新增

    if (!(doctorTabModel->select())) {
        return false;
    }

    theDoctorSelection = new QItemSelectionModel(doctorTabModel);
    return true;
}

int IDatabase::addNewDoctor()
{
    int rowCount = doctorTabModel->rowCount();

    // 插入新行
    bool success = doctorTabModel->insertRow(rowCount);

    if (!success) {
        qDebug() << "插入新行失败";
        return -1;
    }

    // 获取新行的索引
    QModelIndex newIndex = doctorTabModel->index(rowCount, 0);

    // 手动设置ID
    QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    doctorTabModel->setData(newIndex, newId);

    // 自动生成工号
    QString newEmployeeNo = generateNextEmployeeNo();
    if (!newEmployeeNo.isEmpty()) {
        int employeeNoCol = doctorTabModel->fieldIndex("EMPLOYEENO");
        QModelIndex employeeNoIndex = doctorTabModel->index(rowCount, employeeNoCol);
        doctorTabModel->setData(employeeNoIndex, newEmployeeNo);
    }

    return newIndex.row();
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
    departmentTabModel->setSort(departmentTabModel->fieldIndex("NAME"), Qt::AscendingOrder);

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
    visitTabModel = new QSqlTableModel(this, dataBase);
    visitTabModel->setTable("Visit_Record");
    visitTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    visitTabModel->setSort(visitTabModel->fieldIndex("CREATEDTIMESTAMP"), Qt::AscendingOrder);

    // 设置中文表头
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("VISIT_ID"), Qt::Horizontal, "ID");
    // 隐藏ID字段（因为用户不需要看到UUID）
    // visitTabModel->setHeaderData(visitTabModel->fieldIndex("DEPARTMENT_ID"), Qt::Horizontal, "科室ID");
    // visitTabModel->setHeaderData(visitTabModel->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "医生ID");
    // visitTabModel->setHeaderData(visitTabModel->fieldIndex("PATIENT_ID"), Qt::Horizontal, "患者ID");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("DEPARTMENT_NAME"), Qt::Horizontal, "科室名称");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("DOCTOR_NAME"), Qt::Horizontal, "医生姓名");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("PATIENT_NAME"), Qt::Horizontal, "患者姓名");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("VISITTIME"), Qt::Horizontal, "就诊时间");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("SYMPTOMS"), Qt::Horizontal, "症状");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("DIAGNOSIS"), Qt::Horizontal, "诊断结果");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("PRESCRIPTION"), Qt::Horizontal, "处方");
    visitTabModel->setHeaderData(visitTabModel->fieldIndex("CREATEDTIMESTAMP"), Qt::Horizontal, "创建时间");

    if (!(visitTabModel->select())) {
        return false;
    }

    theVisitSelection = new QItemSelectionModel(visitTabModel);

    return true;
}

int IDatabase::addNewVisit()
{
    // visitTabModel->insertRow(visitTabModel->rowCount(), QModelIndex());
    // QModelIndex curIndex = visitTabModel->index(visitTabModel->rowCount() - 1, 1);

    int rowCount = visitTabModel->rowCount();

    // 插入新行
    bool success = visitTabModel->insertRow(rowCount);

    if (!success) {
        qDebug() << "插入新行失败";
        return -1;
    }

    // 获取新行的索引
    QModelIndex newIndex = visitTabModel->index(rowCount, 0);


    // 手动设置ID
    QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    visitTabModel->setData(newIndex, newId);

    return newIndex.row();
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
    query.prepare("SELECT username, password, role FROM user WHERE username = :USER");
    query.bindValue(":USER", userName);
    query.exec();

    if (query.first() && query.value("username").isValid()) {
        QString password = query.value("password").toString();

        if (password == passWord) {
            // 保存当前用户信息
            currentUserName = query.value("username").toString();
            QString roleStr = query.value("role").toString();

            // 转换角色字符串为枚举
            if (roleStr == "Admin") currentUserRole = ROLE_ADMIN;
            else if (roleStr == "Doctor") currentUserRole = ROLE_DOCTOR;
            else if (roleStr == "Patient") currentUserRole = ROLE_PATIENT;
            else currentUserRole = ROLE_PATIENT; // 默认

            qDebug() << "Login ok, user role:" << roleStr;
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

bool IDatabase::userRegister(QString username, QString password, UserRole role)
{
    qDebug() << "=== 开始注册用户 ===";
    qDebug() << "用户名:" << username;
    qDebug() << "密码长度:" << password.length();
    qDebug() << "角色枚举值:" << role;

    // 检查用户名是否已存在
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM user WHERE username = :USERNAME");
    checkQuery.bindValue(":USERNAME", username);

    qDebug() << "执行检查用户名的SQL:" << checkQuery.lastQuery();

    if (!checkQuery.exec()) {
        qDebug() << "检查用户名失败:" << checkQuery.lastError().text();
        qDebug() << "详细错误:" << checkQuery.lastError().databaseText();
        return false;
    }

    int userCount = 0;
    if (checkQuery.next()) {
        userCount = checkQuery.value(0).toInt();
        qDebug() << "数据库中同名用户数量:" << userCount;
    }

    if (userCount > 0) {
        qDebug() << "用户名已存在:" << username;
        return false;
    }

    // 将枚举角色转换为字符串
    QString roleStr;
    switch (role) {
    case ROLE_ADMIN:
        roleStr = "Admin";
        break;
    case ROLE_DOCTOR:
        roleStr = "Doctor";
        break;
    case ROLE_PATIENT:
        roleStr = "Patient";
        break;
    default:
        roleStr = "Patient";
        break;
    }

    qDebug() << "角色字符串:" << roleStr;

    // 插入新用户
    QSqlQuery insertQuery;
    // QString userId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    // qDebug() << "生成的用户ID:" << userId;

    insertQuery.prepare("INSERT INTO user (USERNAME, PASSWORD, ROLE) "
                        "VALUES (:USERNAME, :PASSWORD, :ROLE)");
    // insertQuery.bindValue(":USER_ID", userId);
    insertQuery.bindValue(":USERNAME", username);
    insertQuery.bindValue(":PASSWORD", password);
    insertQuery.bindValue(":ROLE", roleStr);

    qDebug() << "执行插入用户的SQL:" << insertQuery.lastQuery();

    if (!insertQuery.exec()) {
        qDebug() << "插入用户失败:" << insertQuery.lastError().text();
        qDebug() << "详细错误:" << insertQuery.lastError().databaseText();
        return false;
    }

    qDebug() << "注册成功，用户名" << username << "角色:" << roleStr;
    qDebug() << "=== 注册完成 ===";

    return true;
}

QString IDatabase::generateNextEmployeeNo()
{
    QSqlQuery query;

    // 查询当前最大的工号
    // 注意：工号是字符串，需要转换为数字进行排序
    query.exec("SELECT MAX(CAST(EMPLOYEENO AS INTEGER)) as max_no FROM Doctor");

    int maxNo = 0;
    if (query.next()) {
        QVariant maxNoVar = query.value("max_no");
        if (maxNoVar.isValid() && !maxNoVar.isNull()) {
            maxNo = maxNoVar.toInt();
        }
    }

    // 下一个工号
    int nextNo = maxNo + 1;

    // 格式化为4位数，不足前面补0
    return QString("%1").arg(nextNo, 4, 10, QChar('0'));
}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
    , currentUserRole(ROLE_PATIENT)  // 初始化默认角色
    , currentUserName("")
{
    initDatabase();
}
