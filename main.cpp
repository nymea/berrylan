#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QFontDatabase>
#include <QFileInfo>
#include <QDir>
#include <QQuickStyle>
#include <QQmlContext>
#include <QSysInfo>
#include <QtWebView>
#include <QTranslator>
#include <QLibraryInfo>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

#include "wifisetup/bluetoothdiscovery.h"
#include "wifisetup/networkmanagercontroller.h"
#include "wifisetup/wirelessaccesspoints.h"
#include "wifisetup/wirelessaccesspoint.h"
#include "wifisetup/wirelessaccesspointsproxy.h"
#include "wifisetup/wirelesssetupmanager.h"

#include "wifisetupmock/bluetoothdiscoverymock.h"
#include "wifisetupmock/networkmanagercontrollermock.h"
#include "wifisetupmock/wirelessaccesspointsmock.h"
#include "wifisetupmock/wirelessaccesspointmock.h"
#include "wifisetupmock/wirelessaccesspointsproxymock.h"
#include "wifisetupmock/wirelesssetupmanagermock.h"

#include "styles/berrylanbusyindicator.h"
#include "clipboardhelper.h"

int main(int argc, char *argv[])
{

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    QCommandLineOption demoOption("demo");
    parser.addOption(demoOption);
    parser.process(app);

    QtWebView::initialize();
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

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator appTranslator;
    qDebug() << "Loading translation file:" << ":/translations/berrylan-" + QLocale::system().name();
    appTranslator.load(":/translations/berrylan-" + QLocale::system().name());
    app.installTranslator(&appTranslator);

    if (parser.isSet(demoOption)) {
        qmlRegisterType<BluetoothDiscoveryMock>("BerryLan", 1, 0, "BluetoothDiscovery");
        qmlRegisterUncreatableType<BluetoothDeviceInfosMock>("BerryLan", 1, 0, "BluetoothDeviceInfos", "Get it from BluetoothDiscovery");
        qmlRegisterUncreatableType<BluetoothDeviceInfoMock>("BerryLan", 1, 0, "BluetoothDeviceInfo", "Get it from BluetoothDeviceInfos");
        qmlRegisterType<NetworkManagerControllerMock>("BerryLan", 1, 0, "NetworkManagerController");
        qmlRegisterUncreatableType<WirelessSetupManagerMock>("BerryLan", 1, 0, "WirelessSetupManager", "Get it from NeworkManagerController");
        qmlRegisterUncreatableType<WirelessAccessPointsMock>("BerryLan", 1, 0, "WirelessAccessPoints", "Get it from NetworkManagerController");
        qmlRegisterUncreatableType<WirelessAccessPointMock>("BerryLan", 1, 0, "WirelessAccessPoint", "Get it from WirelessAccessPoints");
        qmlRegisterType<WirelessAccessPointsProxyMock>("BerryLan", 1, 0, "WirelessAccessPointsProxy");
    } else {
        qmlRegisterType<BluetoothDiscovery>("BerryLan", 1, 0, "BluetoothDiscovery");
        qmlRegisterUncreatableType<BluetoothDeviceInfos>("BerryLan", 1, 0, "BluetoothDeviceInfos", "Get it from BluetoothDiscovery");
        qmlRegisterUncreatableType<BluetoothDeviceInfo>("BerryLan", 1, 0, "BluetoothDeviceInfo", "Get it from BluetoothDeviceInfos");
        qmlRegisterType<NetworkManagerController>("BerryLan", 1, 0, "NetworkManagerController");
        qmlRegisterUncreatableType<WirelessSetupManager>("BerryLan", 1, 0, "WirelessSetupManager", "Get it from NeworkManagerController");
        qmlRegisterUncreatableType<WirelessAccessPoints>("BerryLan", 1, 0, "WirelessAccessPoints", "Get it from NetworkManagerController");
        qmlRegisterUncreatableType<WirelessAccessPoint>("BerryLan", 1, 0, "WirelessAccessPoint", "Get it from WirelessAccessPoints");
        qmlRegisterType<WirelessAccessPointsProxy>("BerryLan", 1, 0, "WirelessAccessPointsProxy");
    }

    qmlRegisterType<BerryLanBusyIndicator>("BerryLan", 1, 0, "BerryLanBusyIndicator");

    QQmlApplicationEngine engine;

    ClipboardHelper clipBoard;

    engine.rootContext()->setContextProperty("systemProductType", QSysInfo::productType());
    engine.rootContext()->setContextProperty("clipBoard", &clipBoard);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

#ifdef Q_OS_ANDROID
    QtAndroid::hideSplashScreen(250);
#endif

    return app.exec();
}
