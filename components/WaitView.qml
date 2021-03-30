import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    id: root

    property string text
    property string buttonText

    signal buttonClicked()

    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width - app.margins * 2
        spacing: app.margins * 4

        Label {
            Layout.fillWidth: true
            Layout.margins: app.margins
            text: root.text
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font.pixelSize: app.largeFont
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            text: root.buttonText
            visible: root.buttonText != ""
            onClicked: root.buttonClicked()
        }
    }
}
