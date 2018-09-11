import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

ColumnLayout {
    id: root
    spacing: 0

    property string text

    property bool backButtonVisible: false

    signal backClicked()

    RowLayout {
        Layout.margins: app.margins

        ColorIcon {
            Layout.preferredHeight: app.iconSize * .8
            Layout.preferredWidth: height
            name: "../images/back.svg"
            visible: root.backButtonVisible
            MouseArea {
                anchors.fill: parent
                onClicked: root.backClicked()
            }
        }
        Label {
            Layout.fillWidth: true
            Layout.preferredHeight: app.iconSize * .8
            text: root.text.toUpperCase()
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: app.largeFont
            font.family: "Rajdhani"
            font.weight: Font.DemiBold
        }
        Item { // just to keep the label centered
            Layout.preferredHeight: app.iconSize * .8
            Layout.preferredWidth: height
            visible: root.backButtonVisible
        }
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 2
        color: "#D8D8D8"
    }
}

