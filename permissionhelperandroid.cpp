#include "permissionhelperandroid.h"

#include <QHash>
#include <QAndroidIntent>
#include <QApplication>
#include <QSettings>

static QHash<PermissionHelper::Permission, QStringList> permissionMap = {
    {PermissionHelper::PermissionBluetooth, {"android.permission.ACCESS_COARSE_LOCATION", "android.permission.ACCESS_FINE_LOCATION"}},
    {PermissionHelper::PermissionForegroundLocation, {"android.permission.ACCESS_COARSE_LOCATION", "android.permission.ACCESS_FINE_LOCATION"}},
    {PermissionHelper::PermissionBackgroundLocation, {"android.permission.ACCESS_COARSE_LOCATION", "android.permission.ACCESS_FINE_LOCATION", "android.permission.ACCESS_BACKGROUND_LOCATION"}}
};

PermissionHelperAndroid::PermissionHelperAndroid(QObject *parent) : PermissionHelper(parent)
{
    qCDebug(dcPermissionHelper()) << "Creating Permission helper" << this;

    connect(qApp, &QApplication::applicationStateChanged, this, [this](Qt::ApplicationState state){
        if (state == Qt::ApplicationActive) {
            emit bluetoothPermissionChanged();
            emit foregroundLocationPermissionChanged();
            emit backgroundLocationPermissionChanged();
        }
    });
}

PermissionHelperAndroid::~PermissionHelperAndroid()
{
    qCDebug(dcPermissionHelper()) << "Destroying Permission helper" << this;
}

PermissionHelper::PermissionStatus PermissionHelperAndroid::bluetoothPermission() const
{
    return permission(PermissionHelper::PermissionBluetooth);
}

void PermissionHelperAndroid::requestPermission(PermissionHelper::Permission permission)
{
    qCDebug(dcPermissionHelper()) << "Requesting";
    m_requestedPermissions.append(permission);
    QtAndroid::requestPermissions({permissionMap.value(permission)}, &permissionResultCallback);
}

void PermissionHelperAndroid::openPermissionSettings()
{
    qCDebug(dcPermissionHelper()) << "Opening permission settings";

    QAndroidJniObject settingsAction = QAndroidJniObject::getStaticObjectField<jstring>("android/provider/Settings", "ACTION_APPLICATION_DETAILS_SETTINGS");

    QAndroidJniObject packageName = QtAndroid::androidActivity().callObjectMethod("getPackageName", "()Ljava/lang/String;");
    QString packageUri = QString("package:%1").arg(packageName.toString());
    QAndroidJniObject uri = QAndroidJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", QAndroidJniObject::fromString(packageUri).object<jstring>());

    jint flag = QAndroidJniObject::getStaticField<jint>("android/content/Intent", "FLAG_ACTIVITY_NEW_TASK");

    QAndroidJniObject intent("android/content/Intent","(Ljava/lang/String;Landroid/net/Uri;)V", settingsAction.object<jstring>(), uri.object<jobject>());
    intent.callObjectMethod("addCategory", "(Ljava/lang/String;)Landroid/content/Intent;", QAndroidJniObject::fromString("android.intent.category.DEFAULT").object<jstring>());
    intent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", flag);

    QtAndroid::androidActivity().callMethod<void>("startActivity", "(Landroid/content/Intent;)V", intent.object<jobject>());
}

PermissionHelper::PermissionStatus PermissionHelperAndroid::permission(PermissionHelper::Permission permission) const
{
    PermissionStatus status = PermissionStatusGranted;
    QStringList androidPermissions = permissionMap.value(permission);
    foreach (const QString androidPermission, androidPermissions) {
        qCDebug(dcPermissionHelper()) << "Checking permission:" << androidPermission;
        qCDebug(dcPermissionHelper()) << "checked:" << m_requestedPermissions;
        qCDebug(dcPermissionHelper()) << "Done";
        if (QtAndroid::checkPermission(androidPermission) == QtAndroid::PermissionResult::Denied) {
            if (m_requestedPermissions.contains(permission)) {
                return PermissionStatusDenied;
            }
            status = PermissionStatusNotDetermined;
        }
    }
    return status;
}

void PermissionHelperAndroid::permissionResultCallback(const QtAndroid::PermissionResultMap &results)
{
//    qCDebug(dcPermissionHelper()) << "Permission result callback" << results;
    foreach (const QString &key, results.keys()) {
        qCDebug(dcPermissionHelper()) << "Permission result:" << key << (results.value(key) == QtAndroid::PermissionResult::Granted);
    }

    emit PermissionHelper::instance()->bluetoothPermissionChanged();
    emit PermissionHelper::instance()->foregroundLocationPermissionChanged();
    emit PermissionHelper::instance()->backgroundLocationPermissionChanged();
}
