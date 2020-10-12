import QtQuick 2.11
import QtQuick.Controls 2.0

import AppCore 1.0
import TcpIpValidator 1.0

Item {
    id: tcpIpConnectForm
    anchors.fill: parent

    property string tcpIpAddress: tcpIpAddressText.text
    property int normalTextSize: dp(24)
    property int smallTextSize: dp(18)

    //property stringList CUList: []

    Component.onCompleted:
        tcpIpAddressText.text = appcore.lastIpAddress

    signal clicked;
    signal updateControlUnitList;

    onClicked: {
        console.log("connect");
        connectToIpAddress(tcpIpAddress);
    }

    onUpdateControlUnitList: {
        console.log("update list");
        appcore.controlUnits = [];
        controlUnitListDialog.open();
        appcore.updateControlUnitsList();
    }

    TcpIpValidator {
        id: tcpIpValidator
        textToValidate: tcpIpAddressText.text
    }

    TextField {
        id: tcpIpAddressText
        width: parent.width * 0.8
        font.underline: false
        font.pixelSize: normalTextSize
        color: tcpIpValidator.validate ? "black" : "red"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        placeholderText: qsTr("000.000.000.000")
        horizontalAlignment: Text.AlignHCenter
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhPreferNumbers
        onAccepted: parent.clicked()
    }

    Text {
        id: text1
        text: qsTr("Please enter IP-address:")
        font.pixelSize: smallTextSize
        horizontalAlignment: Text.AlignHCenter
        anchors {
            bottom: tcpIpAddressText.top
            horizontalCenter: tcpIpAddressText.horizontalCenter
        }
    }

    Button {
        id: buttonConnect
        text: qsTr("Connect")
        enabled: tcpIpValidator.validate
        autoRepeat: false
        font.pixelSize: normalTextSize
        anchors {
            top: tcpIpAddressText.bottom
            topMargin: smallTextSize * 0.5
            horizontalCenter: tcpIpAddressText.horizontalCenter
        }
        onClicked: parent.clicked()
    }

    Button{
        id: buttonSearch
        text: qsTr("Search...")
        autoRepeat: false;
        font.pixelSize: normalTextSize
        anchors {
            top: buttonConnect.bottom
            topMargin: smallTextSize * 0.5
            horizontalCenter:  tcpIpAddressText.horizontalCenter
        }
        onClicked: parent.updateControlUnitList();
    }

    Drawer {
        id: controlUnitListDialog
        dragMargin: 0
        height: (cuDevList.count*3 + 1.6)*normalTextSize
        width: parent.width;
        edge: Qt.BottomEdge
        clip: true
        Text {
            id: textTitle
            text:"Available Control units:"
            font.pixelSize: normalTextSize
            anchors{
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: normalTextSize * 0.2
            }

        }
        Item {
            width: parent.width
            anchors {
                top: textTitle.bottom
                topMargin: normalTextSize * 0.2
                bottom: parent.bottom
            }
            Component{
                id: cuDelegate
                Item {
                    id: cuListItem
                    height: normalTextSize * 3
                    width: parent.width

                    Rectangle {
                        id: cuListItemButton
                        anchors {
                            fill: parent
                            margins: normalTextSize * 0.2
                        }
                        color: "whitesmoke"
                    }

                    Text {
                        id: typeText
                        text: modelData
                        anchors {
                            horizontalCenter: cuListItemButton.horizontalCenter
                            verticalCenter: cuListItemButton.verticalCenter
                        }
                        font.pixelSize: normalTextSize
                    }

                    MouseArea{
                        anchors.fill: cuListItem
                        onClicked: {
                            tcpIpAddressText.text = modelData;
                            controlUnitListDialog.close();
                            tcpIpConnectForm.clicked();
                        }
                    }
                }
            }
            ListView {
                id: cuDevList
                anchors.fill: parent
                model: appcore.controlUnits
                delegate: cuDelegate
            }
        }

    }
}
