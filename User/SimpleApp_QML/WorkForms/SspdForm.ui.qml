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
            anchors.rightMargin: dp(5)
            anchors.leftMargin: dp(5)
            anchors.bottomMargin: dp(10)
            anchors.topMargin: dp(10)
            anchors.fill: parent
            spacing: dp(2)

            section.property: "group"
            section.criteria: ViewSection.FullString

            section.delegate: Rectangle {
                width: listView.width
                height: childrenRect.height;//pixelSize //* 1.5
                color: "transparent"
                Text {
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    //renderType: Text.NativeRendering
                    font.bold: true
                    text: section
                    font.pixelSize: pixelSize*1.2
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
