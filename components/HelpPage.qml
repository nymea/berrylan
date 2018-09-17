import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtWebView 1.1

Page {
    header: BerryLanHeader {
        text: qsTr("About BerryLan")
        backButtonVisible: true
        onBackClicked: pageStack.pop()
    }

    WebView {
        anchors.fill: parent
        url: "https://www.berrylan.app/faq.html"
    }
}
