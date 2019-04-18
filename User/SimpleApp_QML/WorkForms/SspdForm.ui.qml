import QtQuick 2.4
import QtQuick.Controls 2.3

Item {
    id: uiSspdForm
    anchors.fill: parent

    property alias listView: listView

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true

        ListView {
            id: listView
            anchors.rightMargin: 20
            anchors.leftMargin: 20
            anchors.bottomMargin: 10
            anchors.topMargin: 10
            anchors.fill: parent
            spacing: 2

            section.property: "group"
            section.delegate: Rectangle {
                width: listView.width
                height: pixelSize*1.5
                color: "transparent"
                Text {
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    renderType: Text.NativeRendering
                    font.bold: true
                    text: section
                    font.pixelSize: pixelSize
                }
            }
        }
    }
}
