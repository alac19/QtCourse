#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
class IDatabase : public QObject
{
    Q_OBJECT
public:
    static IDatabase& getInstance()
    {
        static IDatabase    instance;
        return instance;
    }

    QString userLogin(QString userName, QString Password);

private:
    explicit IDatabase(QObject *parent = nullptr);

    void initDatabase();

    IDatabase(IDatabase const&) = delete;
    void operator=(IDatabase const&)  = delete;

    QSqlDatabase dataBase;

signals:
};

#endif // IDATABASE_H
