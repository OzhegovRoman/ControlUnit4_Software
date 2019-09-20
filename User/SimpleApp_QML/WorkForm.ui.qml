import QtQuick 2.4
import QtQuick.Controls 2.0

Item {
    id: uiWelcomeForm
    anchors.fill: parent

    property color headerColor: "#d73c3c"
    property int headerSize: dp(48)
    property alias menuButton: menuButton
    property alias workPageLoader: workPageLoader
    property alias title: title
    property alias menuBackIcon: menuBackIcon

    Rectangle {
        id: menuRect
        height: headerSize //вставить всюду dpi
        color: headerColor
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Text {
            id: title
            color: "white"
            font.pixelSize: dp(24)
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
        Item {
            id: item1
            width: headerSize
            height: headerSize
            anchors.top: parent.top
            anchors.left: parent.left

            MenuBackIcon {
                id: menuBackIcon
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.centerIn: parent.Center
            }
            MouseArea {
                id: menuButton
                anchors.fill: parent
            }
        }
    }
    Loader {
        id: workPageLoader
        anchors.top: menuRect.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
