import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

Item {
    id: root
    implicitHeight: app.iconSize * 4

    property int step: 0

    RowLayout {
        anchors.centerIn: parent
        Rectangle {
            Layout.preferredWidth: app.iconSize * 1.5
            Layout.preferredHeight: app.iconSize * 1.5
            radius: height / 2
//            color: root.step > 1 ? "#e71d36" : "#D8D8D8"
            color: "#D8D8D8"
            Rectangle {
                anchors.centerIn: parent
                height: root.step > 1 ? parent.height : 0
                width: height
                radius: height / 2
                color: "#e71d36"
                Behavior on height {
                    NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
                }
            }

            ColorIcon {
                height: app.iconSize
                width: app.iconSize
                name: "../images/bluetooth.svg"
                anchors.centerIn: parent
                color: "white"
            }
            border.width: root.step > 0 ? 2 : 0
            border.color: "#e71d36"

            MouseArea {
                anchors.fill: parent
                onClicked: root.step--
            }

        }
        Rectangle {
            Layout.preferredWidth: app.iconSize
            Layout.preferredHeight: 4
//            color: root.step > 2 ? "#e71d36" : "#D8D8D8"
            color: "#D8D8D8"
            Rectangle {
                anchors.fill: parent
                anchors.rightMargin:  root.step > 2 ? 0 : parent.width
                color: "#e71d36"
                Behavior on anchors.rightMargin {
                    NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: app.iconSize * 1.5
            Layout.preferredHeight: app.iconSize * 1.5
            radius: height / 2
//            color: root.step > 4 ? "#e71d36" : "#D8D8D8"
            color: "#D8D8D8"
            Rectangle {
                anchors.centerIn: parent
                height: root.step > 4 ? parent.height : 0
                width: height
                radius: height / 2
                color: "#e71d36"
                Behavior on height {
                    NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
                }
            }
            ColorIcon {
                height: app.iconSize
                width: app.iconSize
                name: "../images/wifi.svg"
                anchors.centerIn: parent
                color: "white"
            }
            border.width: root.step > 3 ? 2 : 0
            border.color: "#e71d36"
        }
        Rectangle {
            Layout.preferredWidth: app.iconSize
            Layout.preferredHeight: 4
//            color: root.step > 5 ? "#e71d36" : "#D8D8D8"
            color: "#D8D8D8"
            Rectangle {
                anchors.fill: parent
                anchors.rightMargin:  root.step > 5 ? 0 : parent.width
                color: "#e71d36"
                Behavior on anchors.rightMargin {
                    NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
                }
            }
        }
        Rectangle {
            Layout.preferredWidth: app.iconSize * 1.5
            Layout.preferredHeight: app.iconSize * 1.5
            radius: height / 2
//            color: root.step > 7 ? "#e71d36" : "#D8D8D8"
            color: "#D8D8D8"
            Rectangle {
                anchors.centerIn: parent
                height: root.step > 7 ? parent.height : 0
                width: height
                radius: height / 2
                color: "#e71d36"
                Behavior on height {
                    NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
                }
            }
            ColorIcon {
                height: app.iconSize
                width: app.iconSize
                name: "../images/link.svg"
                anchors.centerIn: parent
                color: "white"
            }
            border.width: root.step > 6 ? 2 : 0
            border.color: "#e71d36"

            MouseArea {
                anchors.fill: parent
                onClicked: root.step++
            }
        }
    }

}
