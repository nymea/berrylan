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
            if (swipeView.currentItem === connectingToWiFiView) {
                if (networkManager.manager.networkStatus === WirelessSetupManager.NetworkStatusGlobal) {
                    swipeView.currentIndex++;
                }
            }
        }
    }

    BerryLanPage {
        anchors.fill: parent
        title: qsTr("Devices")

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
                        onClicked: {
                            networkManager.manager.connectWirelessNetwork(d.currentAP.ssid, passwordTextField.text)
                            swipeView.currentIndex++
//                            pageStack.push(wifiConnectingPageComponent, {ap: authenticationPage.ap, networkManager: authenticationPage.networkManager})
                        }
                    }
                }
            }

            WaitView {
                id: connectingToWiFiView
                height: swipeView.height
                width: swipeView.width
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
                                anchors.horizontalCenter: parent.horizontalCenter
                                name: "../images/github.svg"
                            }
                        }
                    }
                }
            }
        }
    }

    //    StackView {
    //        id: pageStack
    //        anchors.fill: parent
    //        initialItem: discoveryPageComponent
    //    }

    Component {
        id: discoveryPageComponent

        BerryLanPage {
            id: initialPage
            title: qsTr("Devices")
            step: discoveryListView.count === 0 ? 0 : 1

            content: ListView {
                id: discoveryListView
                anchors.fill: parent
                model: discovery.deviceInfos

                delegate: ItemDelegate {
                    width: parent.width
                    text: "bla" + name

                    onClicked: {
                        if (discovery.discovering) {
                            discovery.pendingConnection = index
                            discovery.stop()
                        } else {
                            pageStack.push(connectingPageComponent, { bluetoothDeviceInfo: discovery.deviceInfos.get(index) } )
                        }
                    }
                }

                ColumnLayout {
                    anchors.centerIn: parent
                    width: parent.width - app.margins * 2
                    spacing: app.margins
                    visible: /*discovery.discovering && */discoveryListView.count === 0

                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Searching for your Raspberry Pi")
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                    }

                    BusyIndicator {
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }
        }
    }

    Component {
        id: connectingPageComponent
        BerryLanPage {
            id: connectingPage
            title: "Devices"
            step: 2
            property var bluetoothDeviceInfo

            NetworkManagerController {
                id: networkManager
                bluetoothDeviceInfo: connectingPage.bluetoothDeviceInfo

                Component.onCompleted: networkManager.connectDevice()
            }

            Connections {
                target: networkManager.manager
                onInitializedChanged: {
                    print("initialized changed", networkManager.manager.initialized)
                    if (networkManager.manager.initialized) {
                        pageStack.push(wifiListPageComponent, { networkManager: networkManager } )
                    } else {
                        pageStack.pop()
                    }
                }

                onConnectedChanged: {
                    print("connectedChanged", networkManager.manager.connected)
                    if (!networkManager.manager.connected) {
                        pageStack.pop()
                    }
                }
            }

            content: ColumnLayout {
                anchors.centerIn: parent
                width: parent.width - app.margins * 2
                spacing: app.margins

                Label {
                    Layout.fillWidth: true
                    text: qsTr("Connecting to your Raspberry Pi")
                }
                BusyIndicator {
                    Layout.alignment: Qt.AlignHCenter
                    running: true
                }
            }
        }
    }

    Component {
        id: wifiListPageComponent

        BerryLanPage {
            property QtObject networkManager
            title: qsTr("WLAN")
            step: 3

            content: ListView {
                anchors.fill: parent
                model: networkManager.manager.accessPoints

                delegate: ItemDelegate {
                    width: parent.width
                    text: model.ssid

                    onClicked: {
                        print("Connect to ", model.ssid, " --> ", model.macAddress)
                        pageStack.push(authenticationPageComponent, { ap: networkManager.manager.accessPoints.get(index), networkManager: networkManager })
                    }
                }
            }
        }
    }

    Component {
        id: authenticationPageComponent
        BerryLanPage {
            id: authenticationPage
            title: qsTr("Login")
            step: 4

            property QtObject networkManager: null
            property QtObject ap: null

            content: ColumnLayout {
                anchors.centerIn: parent
                width: app.iconSize * 7
                TextField {
                    id: passwordTextField
                    Layout.fillWidth: true
                    rightPadding: app.iconSize
                    property bool showPassword: false
                    echoMode: showPassword ? TextInput.Normal : TextInput.Password
                    background: Rectangle {
                        radius: 2
                        border.color: "black"
                        border.width: 1
                        ColorIcon {
                            anchors { right: parent.right; verticalCenter: parent.verticalCenter }
                            height: app.iconSize
                            width: app.iconSize
                            name: "../images/eye.svg"
                            color: passwordTextField.showPassword ? "#e71d36" : keyColor
                            MouseArea {
                                anchors.fill: parent
                                onClicked: passwordTextField.showPassword = !passwordTextField.showPassword
                            }
                        }
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: qsTr("OK")
                    onClicked: {
                        networkManager.manager.connectWirelessNetwork(authenticationPage.ap.ssid, passwordTextField.text)
                        pageStack.push(wifiConnectingPageComponent, {ap: authenticationPage.ap, networkManager: authenticationPage.networkManager})
                    }
                }
            }
        }
    }

    Component {
        id: wifiConnectingPageComponent
        BerryLanPage {
            id: wifiConnectingPage
            title: qsTr("Connecting")
            step: {
                switch (wifiConnectingPage.networkManager.manager.networkStatus) {
                case WirelessSetupManager.NetworkStatusUnknown:
                case WirelessSetupManager.NetworkStatusAsleep:
                case WirelessSetupManager.NetworkStatusDisconnected:
                case WirelessSetupManager.NetworkStatusDisconnecting:
                    return 4;
                case WirelessSetupManager.NetworkStatusConnecting:
                    return 5;
                case WirelessSetupManager.NetworkStatusLocal:
                case WirelessSetupManager.NetworkStatusConnectedSite:
                case WirelessSetupManager.NetworkStatusGlobal:
                    return 6;
                }
            }

            property QtObject networkManager: null
            property QtObject ap: null

            Connections {
                target: wifiConnectingPage.networkManager.manager
                onNetworkStatusChanged: {
                    print("wifi status changed", wifiConnectingPage.networkManager.manager.networkStatus)
                    if (wifiConnectingPage.networkManager.manager.networkStatus === WirelessSetupManager.NetworkStatusGlobal) {
                        pageStack.push(finalPageComponent, {ap: wifiConnectingPage.ap, networkManager: wifiConnectingPage.networkManager})
                    }
                }
            }

            content: Item {
                anchors.fill: parent
                BusyIndicator {
                    anchors.centerIn: parent
                }
            }
        }
    }

    Component {
        id: finalPageComponent
        BerryLanPage {
            id: finalPage
            title: qsTr("Connected")
            step: 8
            property QtObject networkManager: null
            property QtObject ap: null

            content: ColumnLayout {
                anchors.fill: parent
                Label {
                    text: "status:" + finalPage.networkManager.manager.networkStatus
                }
                Label {
                    text: "ap:" + ap.hostAddress
                }
            }
        }
    }
}
