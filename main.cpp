#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QFontDatabase>
#include <QFileInfo>
#include <QDir>
#include <QQuickStyle>
#include <QQmlContext>
#include <QSysInfo>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

#include "wifisetup/bluetoothdiscovery.h"
#include "wifisetup/networkmanagercontroller.h"
#include "wifisetup/wirelessaccesspoints.h"
#include "wifisetup/wirelessaccesspoint.h"
#include "wifisetup/wirelessaccesspointsproxy.h"
#include "wifisetup/wirelesssetupmanager.h"
#include "styles/berrylanbusyindicator.h"

int main(int argc, char *argv[])
{

    qmlRegisterType<BluetoothDiscovery>("BerryLan", 1, 0, "BluetoothDiscovery");
    qmlRegisterUncreatableType<BluetoothDeviceInfos>("BerryLan", 1, 0, "BluetoothDeviceInfos", "Get it from BluetoothDiscovery");
    qmlRegisterUncreatableType<BluetoothDeviceInfo>("BerryLan", 1, 0, "BluetoothDeviceInfo", "Get it from BluetoothDeviceInfos");
    qmlRegisterType<NetworkManagerController>("BerryLan", 1, 0, "NetworkManagerController");
    qmlRegisterUncreatableType<WirelessSetupManager>("BerryLan", 1, 0, "WirelessSetupManager", "Get it from NeworkManagerController");
    qmlRegisterUncreatableType<WirelessAccessPoints>("BerryLan", 1, 0, "WirelessAccessPoints", "Get it from NetworkManagerController");
    qmlRegisterUncreatableType<WirelessAccessPoint>("BerryLan", 1, 0, "WirelessAccessPoint", "Get it from WirelessAccessPoints");
    qmlRegisterType<WirelessAccessPointsProxy>("BerryLan", 1, 0, "WirelessAccessPointsProxy");

    qmlRegisterType<BerryLanBusyIndicator>("BerryLan", 1, 0, "BerryLanBusyIndicator");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

//    QQuickStyle::setStyle(":/styles");
    QQuickStyle::setStyle("Material");

    foreach (const QFileInfo &di, QDir(":/fonts/").entryInfoList(QDir::Dirs)) {
        qDebug() << "Adding font path:" << di.absoluteFilePath();
        foreach (const QFileInfo &fi, QDir(di.absoluteFilePath()).entryInfoList()) {
            qDebug() << "Adding font:" << fi.absoluteFilePath();
            QFontDatabase::addApplicationFont(fi.absoluteFilePath());
        }
    }
    QFont applicationFont;
    applicationFont.setFamily("Montserrat");
    applicationFont.setCapitalization(QFont::MixedCase);
    applicationFont.setPixelSize(20);
    applicationFont.setWeight(QFont::Light);
    QGuiApplication::setFont(applicationFont);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("systemProductType", QSysInfo::productType());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

#ifdef Q_OS_ANDROID
    QtAndroid::hideSplashScreen(250);
#endif

    return app.exec();
}
