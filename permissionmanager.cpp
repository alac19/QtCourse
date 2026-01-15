#include "permissionmanager.h"
#include <QMessageBox>

bool PermissionManager::checkPermission(const QString &moduleName)
{
    UserRole role = IDatabase::getInstance().getCurrentUserRole();

    // 权限规则定义
    if (role == ROLE_ADMIN) {
        return true; // 管理员拥有所有权限
    }

    if (moduleName == "department") {
        return role == ROLE_ADMIN; // 只有管理员可以管理科室
    } else if (moduleName == "doctor") {
        return role == ROLE_ADMIN || role == ROLE_DOCTOR; // 管理员和医生可以管理医生信息
    } else if (moduleName == "patient") {
        return role == ROLE_ADMIN || role == ROLE_PATIENT;
    } else if (moduleName == "medicine") {
        return role == ROLE_ADMIN || role == ROLE_DOCTOR; // 药方管理
    } else if (moduleName == "visit") {
        return role == ROLE_ADMIN || role == ROLE_DOCTOR; // 就诊记录
    } else if (moduleName == "appointment") {
        return true; // 所有角色都可以预约
    } else if (moduleName == "schedule") {
        return role == ROLE_ADMIN || role == ROLE_DOCTOR; // 排班管理
    }

    return false;
}

bool PermissionManager::isAdmin() const
{
    return IDatabase::getInstance().getCurrentUserRole() == ROLE_ADMIN;
}

bool PermissionManager::isDoctor() const
{
    return IDatabase::getInstance().getCurrentUserRole() == ROLE_DOCTOR;
}

bool PermissionManager::isPatient() const
{
    return IDatabase::getInstance().getCurrentUserRole() == ROLE_PATIENT;
}

QString PermissionManager::getPermissionDeniedMessage() const
{
    QString roleStr;
    UserRole role = IDatabase::getInstance().getCurrentUserRole();

    switch(role) {
    case ROLE_ADMIN:
        roleStr = "管理员";
        break;
    case ROLE_DOCTOR:
        roleStr = "医生";
        break;
    case ROLE_PATIENT:
        roleStr = "患者";
        break;
    default:
        roleStr = "未知角色";
    }

    return QString("权限不足！当前角色[%1]无法访问此功能。").arg(roleStr);
}

PermissionManager::PermissionManager(QObject *parent)
    : QObject{parent}
{

}
