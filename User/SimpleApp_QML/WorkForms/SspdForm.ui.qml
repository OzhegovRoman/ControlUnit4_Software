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
            anchors.rightMargin: parent.width * 0.05
            anchors.leftMargin: parent.width * 0.05
            anchors.bottomMargin: parent.height * 0.02
            anchors.topMargin: parent.height * 0.02
            anchors.fill: parent
            spacing: pixelSize * 0.2

            section.property: "group"
            section.criteria: ViewSection.FullString

            section.delegate: Rectangle {
                width: listView.width
                height: childrenRect.height
                color: "transparent"
                Text {
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.bold: true
                    text: section
                    font.pixelSize: pixelSize * 1.2
                }
            }
        }
    }
}
