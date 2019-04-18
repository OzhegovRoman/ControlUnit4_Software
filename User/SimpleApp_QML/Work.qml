import QtQuick 2.4
import QtQuick.Controls 2.4

WorkForm {

    property bool isOpened: false
    property int currentDriverAddress: -1

    Component.onCompleted: {
        console.log("WorkForm Completed");
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

    headerSize: 48

    Drawer {
        id: drawer
        dragMargin: 60
        y: headerSize
        width: 0.6 * parent.width
        height: parent.height  - headerSize
        clip: true

        ListView {
            anchors.fill: parent
            delegate: Item {
                height: 1.4* headerSize
                anchors.left: parent.left
                anchors.right: parent.right
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 5
                    color: "whitesmoke"
                    Text {
                        id: typeText
                        text: type
                        anchors.top: parent.top
                        anchors.topMargin: 5
                        anchors.left: parent.left
                        anchors.right: parent.right
                        font.pixelSize: 20
                        renderType: Text.NativeRendering
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Text {
                        text: "Address: " + address
                        anchors.top: typeText.bottom
                        anchors.topMargin: 5
                        anchors.left: parent.left
                        anchors.right: parent.right
                        font.pixelSize: 12
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
        }
    }

    workPageLoader{
        source: "./WorkForms/HomePage.qml"
    }
}
