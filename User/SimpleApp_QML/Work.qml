import QtQuick 2.13
import QtQuick.Controls 2.13

import AppCore 1.0

Item {
    id: workForm

    property color headerColor: "#d73c3c"
    property color textColor: "white"
    property int headerSize: dp(48)
    property int titleTextSize: dp(24)
    property string titleText: ""
    property int currentDriverAddress: -1
    property bool reconnectFlag: false

    Component.onCompleted: {
        console.log("appcore.reconnectEnable: "+appcore.reconnectEnable);
        logOutButton.visible = appcore.reconnectEnable;
    }

    Rectangle {
        id: headerRect
        height: headerSize
        color: headerColor
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }

    Text {
        id: title
        text: titleText
        color: textColor
        font.pixelSize: titleTextSize
        anchors {
            centerIn: headerRect
        }
    }

    MenuBackIcon {
        id: menuBackIcon
        height: headerSize
        width: headerSize
        color: headerColor
        lineColor: textColor
        borderColor: headerColor
        value: drawer.position
        anchors {
            verticalCenter: headerRect.Center
            left: headerRect.left
        }

        onClicked: {
            if (drawer.visible){
                drawer.close()
            }
            else {
                drawer.open()
            }
        }

    }

    Item {
        id: logOutButton
        height: headerSize
        width: headerSize * 0.8
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
            onClicked: {
                if (drawer.visible){
                    drawer.close();
                    reconnectFlag = true;
                }
                else
                    reconnect();
            }
        }
    }


    Drawer {
        id: drawer
        dragMargin: 0.1 * parent.width
        y: headerSize
        width: 0.6 * parent.width
        height: parent.height  - headerSize
        clip: true

        Component{
            id: devDelegate
            Item {
                id: devListItem
                height: headerSize * 1.4
                width: parent.width

                Rectangle {
                    id: devListItemButton
                    anchors{
                        fill: parent
                        margins: headerSize * 0.1
                    }
                    color: "whitesmoke"
                }

                Text {
                    id: typeText
                    text: type
                    anchors {
                        top: devListItemButton.top
                        horizontalCenter: devListItemButton.horizontalCenter
                        topMargin: titleTextSize * 0.3
                    }
                    font.pixelSize: titleTextSize * 0.8
                }

                Text {
                    text: "Address: " + address
                    anchors {
                        top: typeText.bottom
                        topMargin: titleTextSize * 0.1
                        horizontalCenter: devListItemButton.horizontalCenter
                    }
                    font.pixelSize: titleTextSize * 0.5
                }

                MouseArea {
                    anchors.fill: devListItemButton
                    onClicked: {
                        if (type == "Temperature"){
                            currentDriverAddress = address
                            workPageLoader.setSource("./WorkForms/Temperature.qml");
                            titleText = qsTr("Temperature") + " (" + address + ")";
                        }
                        else if (type == "SSPD Driver"){
                            currentDriverAddress = address
                            workPageLoader.setSource("./WorkForms/Sspd.qml");
                            titleText = qsTr("SSPD Driver") + " (" + address + ")";
                        }
                        else {
                            workPageLoader.setSource("./WorkForms/HomePage.qml");
                            titleText = "";
                        }

                        drawer.close();
                    }

                }
            }
        }

        ListView {
            id: devList
            anchors.fill: parent
            model: DeviceModel{
                list: deviceList
            }
            delegate: devDelegate
        }
        onClosed:{
            if (reconnectFlag)
                reconnect();
            console.log("closed");
        }
        onOpened:
            console.log("opened");
    }

    Loader {
        id: workPageLoader
        source: "./WorkForms/HomePage.qml"
        anchors {
            top: headerRect.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
    }
}
