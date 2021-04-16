#ifndef PERMISSIONHELPERANDROID_H
#define PERMISSIONHELPERANDROID_H

#include "permissionhelper.h"

#include <QtAndroid>

class PermissionHelperAndroid : public PermissionHelper
{
    Q_OBJECT
public:
    explicit PermissionHelperAndroid(QObject *parent = nullptr);
    ~PermissionHelperAndroid() override;

    PermissionStatus bluetoothPermission() const override;

    void requestPermission(PermissionHelper::Permission permission) override;
    void openPermissionSettings() override;
signals:

private:
    PermissionStatus permission(PermissionHelper::Permission permission) const;
    static void permissionResultCallback(const QtAndroid::PermissionResultMap &results);

    QList<Permission> m_requestedPermissions;

};

#endif // PERMISSIONHELPERANDROID_H
