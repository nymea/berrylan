import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

ItemDelegate {
    id: root
    height: 60

    leftPadding: app.margins
    rightPadding: app.margins
    topPadding: 0
    bottomPadding: 0

    property string iconSource

    contentItem: ColumnLayout {
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: index === 0 ? 1 : 0
            color: Material.foreground
        }

        RowLayout {
            ColorIcon {
                Layout.preferredHeight: app.iconSize
                Layout.preferredWidth: app.iconSize
                name: root.iconSource
            }

            Label {
                Layout.fillWidth: true
                text: root.text
                font.pixelSize: app.largeFont
                elide: Text.ElideRight
            }

            ColorIcon {
                Layout.preferredHeight: app.iconSize
                Layout.preferredWidth: app.iconSize
                name: "../images/next.svg"
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Material.foreground
        }
    }
}
