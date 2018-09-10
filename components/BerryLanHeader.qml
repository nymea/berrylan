import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

ColumnLayout {
    id: root
    property string text

    RowLayout {
        Layout.topMargin: app.margins

        ColorIcon {
            Layout.preferredHeight: app.iconSize
            Layout.preferredWidth: height
        }
        Label {
            Layout.fillWidth: true
            text: root.text.toUpperCase()
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: app.largeFont
            font.family: "Rajdhani"
            font.weight: Font.DemiBold
        }
        ColorIcon {
            Layout.preferredHeight: app.iconSize
            Layout.preferredWidth: height
        }
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 2
        color: "#D8D8D8"
    }
}

