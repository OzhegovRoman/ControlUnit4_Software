import QtQuick 2.4
import QtQuick.Controls 2.3

Item {
    id: uiSspdForm
    anchors.fill: parent

    property alias listView: listView
    property double dataPart: 0.4

    signal m_toggled

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true

        ListView {
            id: listView
            model: settingsModel

            anchors {
                rightMargin: parent.width * 0.02
                leftMargin: parent.width * 0.02
                bottomMargin: parent.height * 0.01
                topMargin: parent.height * 0.01
                fill: parent
            }
            spacing: pixelSize * 0.2

            section {
                property: "group"
                criteria: ViewSection.FullString
                delegate: Rectangle {
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
}
