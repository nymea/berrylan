#ifndef PERMISSIONHELPERIOS_H
#define PERMISSIONHELPERIOS_H

#include "permissionhelper.h"

#if __OBJC__
@class CBCentralManager;
#else
typedef void CBCentralManager;
#endif


class PermissionHelperIOS : public PermissionHelper
{
    Q_OBJECT
public:
    explicit PermissionHelperIOS(QObject *parent = nullptr);

    PermissionStatus bluetoothPermission() const override;


    void requestPermission(Permission permission) override;
    void openPermissionSettings() override;

    CBCentralManager *m_bluetoothManager = nullptr;
};

#endif // PERMISSIONHELPERIOS_H
