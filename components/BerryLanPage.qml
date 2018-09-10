import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Page {
    id: root
    property int step: 0

    property alias content: contentItemContainer.data

    header: BerryLanHeader {
        text: root.title
    }

    ColumnLayout {
        anchors.fill: parent

        StatusView {
            Layout.fillWidth: true
            step: root.step
        }

        Item {
            id: contentItemContainer
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    footer: Item {
        id: footer
        height: app.iconSize * 2
        RowLayout {
            anchors { bottom: parent.bottom; margins: app.margins; horizontalCenter: parent.horizontalCenter }
            height: app.iconSize
            Label {
                Layout.fillWidth: true
                Layout.preferredHeight: app.iconSize * .8
                text: qsTr("an open source tool by")
                font.pixelSize: app.smallFont
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignBottom
            }
            Image {
                Layout.preferredHeight: app.iconSize
                sourceSize.height: app.iconSize
                source: "../images/nymea.svg"
            }
            Label {
                Layout.preferredHeight: app.iconSize * .8
                text: "n y m e a     "
                font.pixelSize: app.smallFont
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignBottom
            }
        }
        ColorIcon {
            width: app.iconSize
            height: app.iconSize
            anchors { right: parent.right; bottom: parent.bottom; margins: app.margins }
            name: "../images/help.svg"
        }
    }
}
