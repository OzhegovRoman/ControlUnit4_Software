import QtQuick 2.4
import QtQuick.Controls 2.4

WorkForm {
    id: workForm
    signal reconnected
    property bool isOpened: false
    property bool reconnectFlag: false
    property int currentDriverAddress: -1

    headerSize: dp(48)
    titleTextSize: dp(24)


    Component.onCompleted: {
        console.log("WorkForm Completed");
        workForm.reconnected.connect(reconnect)
    }

    menuButton {
        onClicked: {
            console.log("isOpened: "+ isOpened);
            console.log("drawer.opened: "+ drawer.opened);
            if (isOpened){
                console.log("drawer close()");
                drawer.close();
            }
            else {
                console.log("drawer open()");
                drawer.open();
            }
        }
    }

    menuBackIcon.value: drawer.position

    Drawer {
        id: drawer
        dragMargin: dp(60)
        y: headerSize
        width: 0.6 * parent.width
        height: parent.height  - headerSize
        clip: true

        ListView {
            anchors.fill: parent
            delegate: Item {
                height: 1.4 * headerSize
                anchors.left: parent.left
                anchors.right: parent.right
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: dp(5)
                    color: "whitesmoke"
                    Text {
                        id: typeText
                        text: type
                        anchors.top: parent.top
                        anchors.topMargin: dp(5)
                        anchors.left: parent.left
                        anchors.right: parent.right
                        font.pixelSize: dp(20)
                        renderType: Text.NativeRendering
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Text {
                        visible: type != "Reconnect"
                        text: "Address: " + address
                        anchors.top: typeText.bottom
                        anchors.topMargin: dp(5)
                        anchors.left: parent.left
                        anchors.right: parent.right
                        font.pixelSize: dp(12)
                        renderType: Text.NativeRendering
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (type === "Temperature") {
                                currentDriverAddress = address;
                                workPageLoader.setSource("WorkForms/Temperature.qml");
                                title.text = qsTr("Temperature ("+address+")");
                            }
                            else
                            if (type === "SSPD Driver"){
                                currentDriverAddress = address;
                                workPageLoader.setSource("WorkForms/Sspd.qml");
                                title.text = qsTr("SSPD Driver ("+address+")");
                            }
                            else
                            if (type === "Reconnect"){
                                reconnectFlag = true;
                            }
                            else {
                                workPageLoader.setSource("WorkForms/HomePage.qml");
                                title.text = "";
                            }
                            drawer.close();
                        }
                    }

                }
            }
            model: devModel
        }

        onOpened: {
            console.log("drawer onOpened");
            menuBackIcon.state = "back";
            isOpened = true;
        }
        onClosed: {
            console.log("drawer onClosed");
            menuBackIcon.state = "menu";
            isOpened = false;
            if (reconnectFlag)
                reconnected();
        }
    }

    workPageLoader{
        source: "./WorkForms/HomePage.qml"
    }
}
