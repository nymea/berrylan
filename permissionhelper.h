#ifndef PERMISSIONHELPER_H
#define PERMISSIONHELPER_H

#include <QObject>
#include <QQmlEngine>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(dcPermissionHelper)

class PermissionHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PermissionStatus localNetworkPermission READ localNetworkPermission NOTIFY localNetworkPermissionChanged)
    Q_PROPERTY(PermissionStatus bluetoothPermission READ bluetoothPermission NOTIFY bluetoothPermissionChanged)
    Q_PROPERTY(PermissionStatus foregroundLocationPermission READ foregroundLocationPermission NOTIFY foregroundLocationPermissionChanged)
    Q_PROPERTY(PermissionStatus backgroundLocationPermission READ backgroundLocationPermission NOTIFY backgroundLocationPermissionChanged)
    Q_PROPERTY(PermissionStatus notificationsPermission READ notificationsPermission NOTIFY notificationsPermissionChanged)

public:
    enum Permission {
        PermissionNone = 0x00,
        PermissionLocalNetwork = 0x01,
        PermissionBluetooth = 0x02,
        PermissionForegroundLocation = 0x04,
        PermissionBackgroundLocation = 0x08,
        PermissionNotifications = 0x10
    };
    Q_ENUM(Permission)
    Q_DECLARE_FLAGS(Permissions, Permission)
    Q_FLAG(Permissions)

    enum PermissionStatus {
        PermissionStatusNotDetermined,
        PermissionStatusGranted,
        PermissionStatusDenied,
    };
    Q_ENUM(PermissionStatus)

    static QObject* permissionsHelperProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
    static PermissionHelper* instance();
    virtual ~PermissionHelper() override = default;

    virtual PermissionStatus localNetworkPermission() const { return PermissionStatusGranted; }
    virtual PermissionStatus bluetoothPermission() const { return PermissionStatusGranted; }
    virtual PermissionStatus foregroundLocationPermission() const { return PermissionStatusGranted; }
    virtual PermissionStatus backgroundLocationPermission() const { return PermissionStatusGranted; }
    virtual PermissionStatus notificationsPermission() const { return PermissionStatusGranted; }

    Q_INVOKABLE virtual void requestPermission(Permission permission) { Q_UNUSED(permission) }
    Q_INVOKABLE virtual void openPermissionSettings() {}

signals:
    void localNetworkPermissionChanged();
    void bluetoothPermissionChanged();
    void foregroundLocationPermissionChanged();
    void backgroundLocationPermissionChanged();
    void notificationsPermissionChanged();

protected:
    explicit PermissionHelper(QObject *parent = nullptr);

private:
    static PermissionHelper *s_instance;
};

#endif // PERMISSIONHELPER_H
