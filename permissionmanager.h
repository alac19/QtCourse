#ifndef PERMISSIONMANAGER_H
#define PERMISSIONMANAGER_H

#include <QObject>
#include "idatabase.h"

class PermissionManager : public QObject
{
    Q_OBJECT
public:
    static PermissionManager& instance()
    {
        static PermissionManager instance;
        return instance;
    }

    // 检查是否有权限访问模块
    bool checkPermission(const QString& moduleName);

    // 检查当前用户角色
    bool isAdmin() const;
    bool isDoctor() const;
    bool isPatient() const;

    // 获取权限提示信息
    QString getPermissionDeniedMessage() const;

private:
    explicit PermissionManager(QObject *parent = nullptr);
    PermissionManager(const PermissionManager&) = delete;
    PermissionManager& operator=(const PermissionManager&) = delete;

};

#endif // PERMISSIONMANAGER_H
