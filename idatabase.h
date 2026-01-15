#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>


// 角色枚举
enum UserRole {
    ROLE_ADMIN = 1,    // 管理员
    ROLE_DOCTOR = 2,   // 医生
    ROLE_PATIENT = 3   // 患者
};

class IDatabase : public QObject
{
    Q_OBJECT
public:
    static IDatabase& getInstance()
    {
        static IDatabase    instance;
        return instance;
    }

    // 用户登录相关
    QString userLogin(QString userName, QString Password);
    UserRole getCurrentUserRole() const
    {
        return currentUserRole;
    }
    QString getCurrentUserName() const
    {
        return currentUserName;
    }

    // 注册功能
    bool userRegister(QString username, QString password, UserRole role, QString departmentId = "");

    QString generateNextEmployeeNo();  // 新增

private:
    explicit IDatabase(QObject *parent = nullptr);

    void initDatabase();

    IDatabase(IDatabase const&) = delete;
    void operator=(IDatabase const&)  = delete;

    QSqlDatabase dataBase;

    // 当前登录用户信息
    UserRole currentUserRole;
    QString currentUserName;

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

    // 增加科室管理模块数据库部分
    bool initDepartmentModel();                     // 初始化科室模型
    int  addNewDepartment();                        // 添加新科室
    bool searchDepartment(QString filter);          // 搜索科室
    bool deleteCurrentDepartment();                 // 删除当前科室
    bool submitDepartmentEdit();                    // 提交科室编辑
    void revertDepartmentEdit();                    // 撤销科室编辑

    // 增加就诊记录模块数据库部分
    bool initVisitModel();                     // 初始化就诊记录模型
    int  addNewVisit();                        // 添加新就诊记录
    bool searchVisit(QString filter);          // 搜索就诊记录
    bool deleteCurrentVisit();                 // 删除当前就诊记录
    bool submitVisitEdit();                    // 提交就诊记录编辑
    void revertVisitEdit();                    // 撤销就诊记录编辑

    QSqlTableModel *patientTabModel;            // 数据模型
    QItemSelectionModel *thePatientSelection;   // 选择模型

    QSqlTableModel *doctorTabModel;             // 医生数据模型
    QItemSelectionModel *theDoctorSelection;    // 医生选择模型

    QSqlTableModel *departmentTabModel;            // 科室数据模型
    QItemSelectionModel *theDepartmentSelection;   // 科室选择模型

    QSqlTableModel *visitTabModel;            // 就诊记录数据模型
    QItemSelectionModel *theVisitSelection;   // 就诊记录选择模型
};

#endif // IDATABASE_H
