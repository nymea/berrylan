import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
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
        discovering: swipeView.currentIndex <= 1
    }

    NetworkManagerController {
        id: networkManager
    }

    QtObject {
        id: d
        property var currentAP: null
    }

    Connections {
        target: discovery.deviceInfos
        onCountChanged: {
            print("discovery count changed:", discovery.deviceInfos.count)
            if (swipeView.currentItem === discoveringView && discovery.deviceInfos.count > 0) {
                swipeView.currentIndex++
            }
        }
    }

    Connections {
        target: networkManager.manager
        onInitializedChanged: {
            print("initialized changed", networkManager.manager.initialized)

            if (networkManager.manager.initialized) {
                swipeView.currentIndex++;
            } else {
                swipeView.currentIndex = 0;
            }
        }

        onConnectedChanged: {
            print("connectedChanged", networkManager.manager.connected)
            if (!networkManager.manager.connected) {
                swipeView.currentIndex = 0;
            }
        }

        onNetworkStatusChanged: {
            print("Network status changed:", networkManager.manager.networkStatus)
            if (swipeView.currentItem === connectingToWiFiView) {
                if (networkManager.manager.networkStatus === WirelessSetupManager.NetworkStatusGlobal) {
                    swipeView.currentIndex++;
                } else {
                    print("UNHANDLED Network status change:", networkManager.manager.networkStatus  )
                }

            }
        }
        onWirelessStatusChanged: {
            print("Wireless status changed:", networkManager.manager.networkStatus)
            if (swipeView.currentItem === connectingToWiFiView) {
                if (networkManager.manager.wirelessStatus === WirelessSetupManager.WirelessStatusActivated) {
                    swipeView.currentIndex++;
                }
                if (networkManager.manager.wirelessStatus === WirelessSetupManager.WirelessStatusFailed) {
                    connectingToWiFiView.running = false
                    connectingToWiFiView.text = qsTr("Sorry, the password is wrong.")
                    connectingToWiFiView.buttonText = qsTr("Try again")
                }
            }
        }
    }

    StackView {
        id: pageStack
        anchors.fill: parent
        initialItem: BerryLanPage {
            title: qsTr("Devices")
            backButtonVisible: swipeView.currentIndex === 4

            onHelpClicked: pageStack.push(Qt.resolvedUrl("components/HelpPage.qml"))
            onBackClicked: swipeView.currentIndex--

            step: {
                switch (swipeView.currentIndex) {
                case 0:
                    return 0;
                case 1:
                    return 1;
                case 2:
                    return 2;
                case 3:
                    if (!networkManager.manager || networkManager.manager.accessPoints.count == 0) {
                        return 3;
                    }
                    return 4;
                case 4:
                    return 4;
                case 5:
                    if (networkManager.manager.wirelessStatus < WirelessSetupManager.WirelessStatusPrepare) {
                        return 5;
                    }
                    if (networkManager.manager.wirelessStatus < WirelessSetupManager.WirelessStatusIpConfig) {
                        return 6;
                    }
                    return 7;
                case 6:
                    return 8;
                }
            }

            content: SwipeView {
                id: swipeView
                anchors.fill: parent
                interactive: false

                WaitView {
                    id: discoveringView
                    height: swipeView.height
                    width: swipeView.width
                    text: qsTr("Searching for your Raspberry Pi")
                }

                ListView {
                    id: discoveryListView
                    height: swipeView.height
                    width: swipeView.width
                    model: discovery.deviceInfos
                    clip: true

                    delegate: BerryLanItemDelegate {
                        width: parent.width
                        text: name
                        iconSource: "../images/bluetooth.svg"

                        onClicked: {
                            networkManager.bluetoothDeviceInfo = discovery.deviceInfos.get(index);
                            networkManager.connectDevice();
                            swipeView.currentIndex++;
                        }
                    }
                }

                WaitView {
                    id: connectingToPiView
                    height: swipeView.height
                    width: swipeView.width
                    text: qsTr("Connecting to your Raspberry Pi")
                }

                ListView {
                    id: apSelectionListView
                    height: swipeView.height
                    width: swipeView.width
                    model: networkManager.manager ? networkManager.manager.accessPoints : null
                    clip: true

                    delegate: BerryLanItemDelegate {
                        width: parent.width
                        text: model.ssid
                        iconSource: "../images/wifi.svg"

                        onClicked: {
                            print("Connect to ", model.ssid, " --> ", model.macAddress)
                            d.currentAP = networkManager.manager.accessPoints.get(index);

                            swipeView.currentIndex++;
                        }
                    }
                }

                Item {
                    id: authenticationView
                    width: swipeView.width
                    height: swipeView.height
                    ColumnLayout {
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: - swipeView.height / 4
                        width: app.iconSize * 8
                        spacing: app.margins
                        RowLayout {
                            TextField {
                                id: passwordTextField
                                Layout.fillWidth: true
                                property bool showPassword: false
                                echoMode: showPassword ? TextInput.Normal : TextInput.Password
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
                            Layout.fillWidth: true
                            text: qsTr("OK")
                            enabled: passwordTextField.displayText.length >= 8
                            onClicked: {
                                connectingToWiFiView.text = qsTr("Connecting the Raspberry Pi to %1").arg(d.currentAP.ssid);
                                connectingToWiFiView.buttonText = "";
                                connectingToWiFiView.running = true
                                networkManager.manager.connectWirelessNetwork(d.currentAP.ssid, passwordTextField.text)
                                swipeView.currentIndex++
                            }
                        }
                    }
                }

                WaitView {
                    id: connectingToWiFiView
                    height: swipeView.height
                    width: swipeView.width
                    onButtonClicked: {
                        swipeView.currentIndex--;
                    }
                }

                Item {
                    id: resultsView
                    height: swipeView.height
                    width: swipeView.width
                    ColumnLayout {
                        anchors.fill: parent
                        Label {
                            Layout.fillWidth: true
                            text: d.currentAP ? "IP Address: " + d.currentAP.hostAddress : ""
                            font.pixelSize: app.largeFont
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
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
                                            Qt.openUrlExternally("https://github.com/guh/berrylan")
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
}
