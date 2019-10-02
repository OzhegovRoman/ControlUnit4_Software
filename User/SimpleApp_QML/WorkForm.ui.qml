import QtQuick 2.4
import QtQuick.Controls 2.0

import AppCore 1.0

Item {
    id: uiWelcomeForm
    anchors.fill: parent

    property color headerColor: "#d73c3c"
    property int headerSize: 48
    property int titleTextSize: 24
    property alias menuButton: menuButton
    property alias logoutButton: logoutButton

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
            font.pixelSize: titleTextSize
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

        Item {
            visible: AppCore.reconnectEnable
            id: item2
            height: headerSize //вставить всюду dpi
            width: headerSize * 0.8 //вставить всюду dpi
            anchors {
                right: parent.right
                rightMargin: headerSize * 0.3
                top: parent.top
            }
            Image {
                id: logOutImage
                anchors {
                    fill: parent
                    margins: headerSize * 0.2
                }
                fillMode: Image.Stretch

                source: "png/LogOut.png"
            }

            MouseArea {
                id: logoutButton
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

