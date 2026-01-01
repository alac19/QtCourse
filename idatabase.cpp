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
    }
    else {
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
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));

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

}

int IDatabase::addNewDoctor()
{

}

bool IDatabase::searchDoctor(QString filter)
{

}

bool IDatabase::deleteCurrentDoctor()
{

}

bool IDatabase::submitDoctorEdit()
{

}

void IDatabase::revertDoctorEdit()
{

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
        }
        else {
            qDebug() << "wrong password";
            return "wrongPassword";
        }
    }
    else {
        qDebug() << "no such user";
        return "wrongUsername";
    }
}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    initDatabase();
}
