import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import Qt.labs.settings 1.1
import BerryLan 1.0
import "components"

ApplicationWindow {
    id: app
    visible: true
    width: 320
    height: 480
    title: qsTr("BerryLan")

    Material.foreground: "#011627"
    Material.background: "#fdfffc"
    Material.accent: "#e71d36"

    property int iconSize: 32
    property int margins: 6
    property int largeFont: 20
    property int smallFont: 12

    BluetoothDiscovery {
        id: discovery
        discoveryEnabled: PermissionHelper.bluetoothPermission === PermissionHelper.PermissionStatusGranted && swipeView.currentIndex <= 1
    }

    BluetoothDeviceInfosProxy {
        id: discoveryProxy
        model: discovery.deviceInfos
        nameWhitelist: ["BT WLAN setup"]
        filterForLowEnergy: true
        filterForServiceUUID: settings.filterEnabled ? "e081fec0-f757-4449-b9c9-bfa83133f7fc" : ""
    }

    BtWiFiSetup {
        id: btWiFiSetup
        onBluetoothStatusChanged: {
            print("status changed", bluetoothStatus)
            switch (bluetoothStatus) {
            case BtWiFiSetup.BluetoothStatusLoaded:
                swipeView.currentIndex = 3
                break;
            case BtWiFiSetup.BluetoothStatusDisconnected:
                swipeView.currentIndex = 1
                break;
            }
        }
        onBluetoothConnectionError: {
            print("bt connection error")
            swipeView.currentIndex = 1
        }
        onWirelessStatusChanged: {
            if (wirelessStatus == BtWiFiSetup.WirelessStatusFailed) {
                connectingToWiFiView.text = qsTr("Sorry, the password is wrong.")
                connectingToWiFiView.buttonText = qsTr("Try again")
                swipeView.currentIndex = 5
            } else if (wirelessStatus == BtWiFiSetup.WirelessStatusActivated) {
                swipeView.currentIndex = 6
            }
        }
    }

    QtObject {
        id: d
        property var currentAP: null
        readonly property bool accessPointMode: btWiFiSetup.wirelessDeviceMode == BtWiFiSetup.WirelessDeviceModeAccessPoint
    }

    Settings {
        id: settings
        property bool filterEnabled: true
    }

    StackView {
        id: pageStack
        anchors.fill: parent
        initialItem: BerryLanPage {
            title: {
                switch (swipeView.currentIndex) {
                case 0:
                case 1:
                case 2:
                    return qsTr("Devices")
                case 3:
                    return qsTr("Network")
                case 4:
                    return qsTr("Login")
                case 5:
                    return qsTr("Connecting")
                case 6:
                    return qsTr("Connected")
                }
            }

            settingsButtonVisible: swipeView.currentIndex <= 1
            backButtonVisible: swipeView.currentIndex === 4

            onHelpClicked: pageStack.push(Qt.resolvedUrl("components/HelpPage.qml"))
            onBackClicked: {
                d.currentAP = null
                swipeView.currentIndex--
            }
            onSettingsClicked: {
                var popup = settingsComponent.createObject(app)
                popup.open()
            }

            step: {
                switch (swipeView.currentIndex) {
                case 0:
                case 1:
                    return 0;
                case 2:
                    return 3;
                case 3:
                    if (!btWiFiSetup) {
                        return 2;
                    }
                    if (btWiFiSetup.accessPoints.count == 0) {
                        return 3;
                    }
                    return 4;
                case 4:
                    return 4;
                case 5:
                    return 6;
                case 6:
                    return 8;
                }
            }

            content: SwipeView {
                id: swipeView
                anchors.fill: parent
                interactive: false

                currentIndex: {
                    if (discoveryProxy.count == 0) {
                        return 0;
                    }
                    return 1;
                }

                // 0
                WaitView {
                    id: discoveringView
                    height: swipeView.height
                    width: swipeView.width
                    text: !discovery.bluetoothAvailable
                          ? qsTr("Bluetooth doesn't seem to be available on this device. BerryLan requires a working Bluetooth connection.")
                          : !discovery.bluetoothEnabled
                            ? qsTr("Bluetooth seems to be disabled. Please enable Bluetooth on your device in order to use BerryLan.")
                            : PermissionHelper.bluetoothPermission === PermissionHelper.PermissionStatusNotDetermined
                              ? qsTr("Berrylan doesn't seem to have permissions to scan for Bluetooth devices.")
                              : PermissionHelper.bluetoothPermission === PermissionHelper.PermissionStatusDenied
                                ? qsTr("Bluetooth access has been denied. Please grant permissions in the settings.")
                                  : qsTr("Searching for your\nRaspberry Pi")

                    buttonText: PermissionHelper.bluetoothPermission === PermissionHelper.PermissionStatusNotDetermined
                                ? qsTr("Request permission")
                                : PermissionHelper.bluetoothPermission === PermissionHelper.PermissionStatusDenied
                                  ? qsTr("Open settings")
                                  : ""
                    onButtonClicked: {
                        print("BT permission", PermissionHelper.bluetoothPermission)
                        if (PermissionHelper.bluetoothPermission === PermissionHelper.PermissionStatusNotDetermined) {
                            PermissionHelper.requestPermission(PermissionHelper.PermissionBluetooth)
                        } else {
                            PermissionHelper.openPermissionSettings();
                        }
                    }
                }

                // 1
                ListView {
                    id: discoveryListView
                    height: swipeView.height
                    width: swipeView.width
                    model: discoveryProxy
                    clip: true

                    delegate: BerryLanItemDelegate {
                        width: parent.width
                        text: name
                        iconSource: "../images/bluetooth.svg"

                        onClicked: {
                            btWiFiSetup.connectToDevice(discoveryProxy.get(index));
                            swipeView.currentIndex++;
                        }
                    }
                }

                // 2
                WaitView {
                    id: connectingToPiView
                    height: swipeView.height
                    width: swipeView.width
                    text: qsTr("Connecting to your Raspberry Pi")
                }

                // 3
                ColumnLayout {
                    height: swipeView.height
                    width: swipeView.width

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        id: apSelectionListView
                        model: WirelessAccessPointsProxy {
                            id: accessPointsProxy
                            accessPoints: btWiFiSetup.accessPoints
                        }
                        clip: true

                        delegate: BerryLanItemDelegate {
                            width: parent.width
                            text: model.ssid.length > 0 ? model.ssid : qsTr("Hidden network")
                            iconSource: model.signalStrength > 66
                                        ? "../images/wifi-100.svg"
                                        : model.signalStrength > 33
                                          ? "../images/wifi-66.svg"
                                          : model.signalStrength > 0
                                            ? "../images/wifi-33.svg"
                                            : "../images/wifi-0.svg"

                            onClicked: {
                                print("Connect to ", model.ssid, " --> ", model.macAddress)
                                d.currentAP = accessPointsProxy.get(index);

                                if (!d.currentAP.isProtected) {
                                    btWiFiSetup.manager.connectWirelessNetwork(d.currentAP.ssid)
                                    swipeView.currentIndex++;
                                }
                                swipeView.currentIndex++;
                            }
                        }
                    }

                    Button {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Open Access Point")
                        onClicked: {
                            swipeView.currentIndex++
                        }
                    }
                }


                // 4
                Item {
                    id: authenticationView
                    width: swipeView.width
                    height: swipeView.height
                    ColumnLayout {
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: - swipeView.height / 4
                        width: app.iconSize * 8
                        spacing: app.margins
                        Label {
                            Layout.fillWidth: true
                            text: qsTr("Name")
                            visible: !d.currentAP
                        }

                        TextField {
                            id: ssidTextField
                            Layout.fillWidth: true
                            visible: !d.currentAP
                            maximumLength: 32
                            onAccepted: {
                                passwordTextField.focus = true
                            }
                        }

                        Label {
                            Layout.fillWidth: true
                            text: qsTr("Password")
                        }

                        RowLayout {
                            TextField {
                                id: passwordTextField
                                Layout.fillWidth: true
                                maximumLength: 64
                                property bool showPassword: false
                                echoMode: showPassword ? TextInput.Normal : TextInput.Password
                                onAccepted: {
                                    okButton.clicked()
                                }
                            }

                            ColorIcon {
                                Layout.preferredHeight: app.iconSize
                                Layout.preferredWidth: app.iconSize
                                name: "../images/eye.svg"
                                color: passwordTextField.showPassword ? "#e71d36" : keyColor
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: passwordTextField.showPassword = !passwordTextField.showPassword
                                }
                            }
                        }

                        Button {
                            id: okButton
                            Layout.fillWidth: true
                            text: qsTr("OK")
                            enabled: passwordTextField.displayText.length >= 8
                            onClicked: {
                                if (d.currentAP) {
                                    connectingToWiFiView.text = qsTr("Connecting the Raspberry Pi to %1").arg(d.currentAP.ssid);
                                    btWiFiSetup.connectDeviceToWiFi(d.currentAP.ssid, passwordTextField.text)
                                } else {
                                    connectingToWiFiView.text = qsTr("Opening access point \"%1\" on the Raspberry Pi").arg(ssidTextField.text);
                                    btWiFiSetup.startAccessPoint(ssidTextField.text, passwordTextField.text)
                                }
                                connectingToWiFiView.buttonText = "";

                                swipeView.currentIndex++
                            }
                        }
                    }
                }

                // 5
                WaitView {
                    id: connectingToWiFiView
                    height: swipeView.height
                    width: swipeView.width
                    onButtonClicked: {
                        swipeView.currentIndex--;
                        if (!d.currentAP.isProtected) {
                            swipeView.currentIndex--;
                        }
                    }
                }

                // 6
                Item {
                    id: resultsView
                    height: swipeView.height
                    width: swipeView.width

                    ColumnLayout {
                        anchors.fill: parent
                        Label {
                            Layout.fillWidth: true
                            Layout.margins: app.margins
                            text: d.accessPointMode
                                  ? qsTr("Access point name: %1").arg(btWiFiSetup.currentConnection.ssid)
                                  : btWiFiSetup.currentConnection ? qsTr("IP Address: %1").arg(btWiFiSetup.currentConnection.hostAddress) : ""
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.pixelSize: app.largeFont
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    clipBoard.text = btWiFiSetup.currentConnection.hostAddress
                                    parent.ToolTip.show(qsTr("IP address copied to clipboard."), 2000)
                                }
                            }
                        }
                        Button {
                            Layout.alignment: Qt.AlignHCenter
                            visible: d.accessPointMode
                            text: qsTr("Close access point")
                            onClicked: {
                                btWiFiSetup.disconnectDeviceFromWiFi();
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            ColumnLayout {
                                anchors.centerIn: parent
                                width: parent.width
                                spacing: app.margins * 4
                                Label {
                                    text: qsTr("Thanks for using BerryLan!")
                                    font.pixelSize: app.largeFont
                                    Layout.fillWidth: true
                                    wrapMode: Text.WordWrap
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                ColorIcon {
                                    Layout.preferredHeight: app.iconSize
                                    Layout.preferredWidth: app.iconSize
                                    Layout.alignment: Qt.AlignHCenter
                                    name: "../images/github.svg"

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            Qt.openUrlExternally("https://github.com/nymea/berrylan")
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: settingsComponent
        Dialog {
            id: settingsDialog
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 300
            title: qsTr("Settings")
            contentItem: ColumnLayout {
                width: parent.width
                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("BerryLan tries to detect the network setup capabilities on the devices it finds. However, old installations might not suport this. Disable filtering if your devices aren't being found.")
                    font.pixelSize: app.smallFont
                }

                CheckDelegate {
                    Layout.fillWidth: true
                    text: qsTr("Filtering enabled")
                    checked: settings.filterEnabled
                    onClicked: settings.filterEnabled = !settings.filterEnabled
                }

                Button {
                    Layout.fillWidth: true
                    text: qsTr("OK")
                    onClicked: {
                        settingsDialog.close()
                    }
                }
            }
        }
    }
}
