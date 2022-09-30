#include "permissionhelper.h"

#ifdef Q_OS_ANDROID
#include "permissionhelperandroid.h"
#elif defined(Q_OS_IOS)
#include "permissionhelperios.h"
#endif

Q_LOGGING_CATEGORY(dcPermissionHelper, "Permissions")

PermissionHelper* PermissionHelper::s_instance = nullptr;

PermissionHelper *PermissionHelper::instance()
{
    if (!s_instance) {
#ifdef Q_OS_ANDROID
        s_instance = new PermissionHelperAndroid();
#elif defined(Q_OS_IOS)
        s_instance = new PermissionHelperIOS();
#else
        s_instance = new PermissionHelper();
#endif
    }
    return s_instance;
}

QObject *PermissionHelper::permissionsHelperProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return instance();
}

PermissionHelper::PermissionHelper(QObject *parent) : QObject(parent)
{

}

