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

public:
    bool initPatientModel();
    int  addNewPatient();
    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void revertPatientEdit();

    // 增加医生管理模块数据库部分
    bool initDoctorModel();                     // 初始化医生模型
    int  addNewDoctor();                        // 添加新医生
    bool searchDoctor(QString filter);          // 搜索医生
    bool deleteCurrentDoctor();                 // 删除当前医生
    bool submitDoctorEdit();                    // 提交医生编辑
    void revertDoctorEdit();                    // 撤销医生编辑

    QSqlTableModel *patientTabModel;            // 数据模型
    QItemSelectionModel *thePatientSelection;   // 选择模型

    QSqlTableModel *doctorTabModel;             // 医生数据模型
    QItemSelectionModel *theDoctorSelection;    // 医生选择模型
};

#endif // IDATABASE_H
