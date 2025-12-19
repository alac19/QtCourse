#include "idatabase.h"

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
