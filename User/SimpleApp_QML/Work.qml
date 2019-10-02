import QtQuick 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

WorkForm {
    id: workForm
    signal reconnected
    property bool isOpened: false
    property bool reconnectFlag: false
    property int currentDriverAddress: -1

    headerSize: dp(48)
    titleTextSize: dp(24)

    Component.onCompleted: {
        workForm.reconnected.connect(reconnect)
    }

    menuButton {
        onClicked: {
            if (isOpened){
                drawer.close();
            }
            else {
                drawer.open();
            }
        }
    }

    logoutButton {
        onClicked: {
            if (isOpened){
                reconnectFlag = true
                drawer.close();
            }
            else
                reconnected();
        }
    }

    menuBackIcon.value: drawer.position

    Drawer {
        id: drawer
        dragMargin: 0.1 * parent.width
        y: headerSize
        width: 0.6 * parent.width
        height: parent.height  - headerSize
        clip: true

        Component{
            id: devListDelegate
            Item{
                id: devListItem
                height: headerSize*1.4
                width: parent.width
                Rectangle{
                    id: button
                    anchors{
                        fill: parent
                        margins: headerSize * 0.1
                    }
                    color: "whitesmoke"
                }
                Text {
                    visible: type != "Reconnect"
                    id: typeText
                    text: type
                    anchors {
                        top: button.top
                        horizontalCenter: button.horizontalCenter
                        topMargin: titleTextSize * 0.3
                    }
                    font.pixelSize: titleTextSize * 20/24
                }
                Text {
                    visible: type != "Reconnect"
                    text: "Address: " + address
                    anchors {
                        top: typeText.bottom
                        topMargin: titleTextSize * 0.1
                        horizontalCenter: button.horizontalCenter
                    }
                    font.pixelSize: titleTextSize * 0.5
                }
                Text {
                    visible: type === "Reconnect"
                    text: type
                    font.pixelSize: titleTextSize * 20/24
                    anchors {
                        horizontalCenter:   devListItem.horizontalCenter
                        verticalCenter:     devListItem.verticalCenter
                    }
                }
                MouseArea {
                    anchors.fill: devListItem
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

        ListView {
            id: deviceListView
            anchors.fill: parent
            model: devModel
            delegate: devListDelegate
        }

        onOpened: {
            menuBackIcon.state = "back";
            isOpened = true;
        }
        onClosed: {
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
