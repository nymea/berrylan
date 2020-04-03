QT += quick bluetooth svg quickcontrols2
QT += quickcontrols2-private webview
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# read version info from version.txt
VERSION_INFO=$$cat(version.txt)
APP_VERSION=$$member(VERSION_INFO, 0)
APP_REVISION=$$member(VERSION_INFO, 1)

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clipboardhelper.cpp \
    main.cpp \
    styles/berrylanbusyindicator.cpp \
    wifisetup/bluetoothdevice.cpp \
    wifisetup/bluetoothdeviceinfo.cpp \
    wifisetup/bluetoothdeviceinfos.cpp \
    wifisetup/bluetoothdiscovery.cpp \
    wifisetup/wirelessaccesspoint.cpp \
    wifisetup/wirelessaccesspoints.cpp \
    wifisetup/wirelessaccesspointsproxy.cpp \
    wifisetup/wirelesssetupmanager.cpp \
    wifisetup/networkmanagercontroller.cpp \

SOURCES += \
    wifisetupmock/networkmanagercontrollermock.cpp \
    wifisetupmock/bluetoothdiscoverymock.cpp \
    wifisetupmock/bluetoothdevicemock.cpp \
    wifisetupmock/bluetoothdeviceinfosmock.cpp \
    wifisetupmock/bluetoothdeviceinfomock.cpp \
    wifisetupmock/wirelessaccesspointmock.cpp \
    wifisetupmock/wirelessaccesspointsmock.cpp \
    wifisetupmock/wirelessaccesspointsproxymock.cpp \
    wifisetupmock/wirelesssetupmanagermock.cpp \

HEADERS += \
    clipboardhelper.h \
    styles/berrylanbusyindicator.h \
    wifisetup/bluetoothdevice.h \
    wifisetup/bluetoothdeviceinfo.h \
    wifisetup/bluetoothdeviceinfos.h \
    wifisetup/bluetoothdiscovery.h \
    wifisetup/wirelessaccesspoint.h \
    wifisetup/wirelessaccesspoints.h \
    wifisetup/wirelessaccesspointsproxy.h \
    wifisetup/wirelesssetupmanager.h \
    wifisetup/networkmanagercontroller.h \

HEADERS += \
    wifisetupmock/networkmanagercontrollermock.h \
    wifisetupmock/bluetoothdiscoverymock.h \
    wifisetupmock/bluetoothdevicemock.h \
    wifisetupmock/bluetoothdeviceinfosmock.h \
    wifisetupmock/bluetoothdeviceinfomock.h \
    wifisetupmock/wirelessaccesspointmock.h \
    wifisetupmock/wirelessaccesspointsmock.h \
    wifisetupmock/wirelessaccesspointsproxymock.h \
    wifisetupmock/wirelesssetupmanagermock.h \

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

android: {
    QT += androidextras
    DISTFILES += \
        android/AndroidManifest.xml \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradlew \
        android/res/values/libs.xml \
        android/build.gradle \
        android/gradle/wrapper/gradle-wrapper.properties \
        android/gradlew.bat

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    QMAKE_POST_LINK += cp $$PWD/version.txt $$OUT_PWD/
}

ios: {
    QMAKE_TARGET_BUNDLE_PREFIX = io.guh
    QMAKE_BUNDLE = berrylan
    # Configure generated xcode project to have our bundle id
    xcode_product_bundle_identifier_setting.value = $${QMAKE_TARGET_BUNDLE_PREFIX}.$${QMAKE_BUNDLE}
    QMAKE_ASSET_CATALOGS += ios/AppIcons.xcassets
    plist.input = ios/Info.plist.in
    plist.output = $$OUT_PWD/Info.plist
    QMAKE_SUBSTITUTES += plist
    QMAKE_INFO_PLIST = $$OUT_PWD/Info.plist
    OTHER_FILES += ios/Info.plist.in
    ios_launch_images.files += $$files(ios/LaunchImage*.png) ios/LaunchScreen1.xib
    QMAKE_BUNDLE_DATA += ios_launch_images

    ios_icon_files.files += $$files(ios/AppIcons.xcassets/AppIcon.appiconset/AppIcon*.png)
    QMAKE_BUNDLE_DATA += ios_icon_files
}

TRANSLATIONS += $$files($$absolute_path(translations)/*.ts, true)
# Run lrelease with the qmake run so we're ready to build
system("lrelease $$TRANSLATIONS")

# and add make targets for manual lupdate and lrelease runs
for(ts, $$list($${SOURCES})) { ABS_SOURCES += $$absolute_path($${ts}) }
for(qrc, $$list($${RESOURCES})) { ABS_RESOURCES += $$absolute_path($${qrc}) }
lupdate.commands = lupdate $${ABS_SOURCES} $${ABS_RESOURCES} -ts $${TRANSLATIONS}
lrelease.commands = lrelease $${TRANSLATIONS}
QMAKE_EXTRA_TARGETS += lupdate lrelease
