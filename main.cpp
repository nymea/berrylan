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
#include "wifisetup/btwifisetup.h"
#include "wifisetup/wirelessaccesspoint.h"
#include "wifisetup/wirelessaccesspoints.h"
#include "wifisetup/wirelessaccesspointsproxy.h"

#include "clipboardhelper.h"
#include "permissionhelper.h"

int main(int argc, char *argv[])
{

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    app.setApplicationName("berrylan");
    app.setOrganizationName("nymea");

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

    qmlRegisterType<BtWiFiSetup>("BerryLan", 1, 0, "BtWiFiSetup");
    qmlRegisterType<BluetoothDiscovery>("BerryLan", 1, 0, "BluetoothDiscovery");
    qmlRegisterUncreatableType<BluetoothDeviceInfos>("BerryLan", 1, 0, "BluetoothDeviceInfos", "Get it from BluetoothDiscovery");
    qmlRegisterType<BluetoothDeviceInfosProxy>("BerryLan", 1, 0, "BluetoothDeviceInfosProxy");
    qmlRegisterUncreatableType<BluetoothDeviceInfo>("BerryLan", 1, 0, "BluetoothDeviceInfo", "Get it from BluetoothDeviceInfos");
    qmlRegisterUncreatableType<WirelessAccessPoints>("BerryLan", 1, 0, "WirelessAccessPoints", "Get it from NetworkManagerController");
    qmlRegisterUncreatableType<WirelessAccessPoint>("BerryLan", 1, 0, "WirelessAccessPoint", "Get it from WirelessAccessPoints");
    qmlRegisterType<WirelessAccessPointsProxy>("BerryLan", 1, 0, "WirelessAccessPointsProxy");

    qmlRegisterSingletonInstance<PermissionHelper>("BerryLan", 1, 0, "PermissionHelper", PermissionHelper::instance());

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
