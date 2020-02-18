import QtQuick 2.13
import QtQuick.Controls 2.13

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
        deviceListDialog.open();
        updateControlUnitsList();
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
        id: deviceListDialog
        dragMargin: 0
        height: parent.height*0.8;
        width: parent.width;
        edge: Qt.BottomEdge
        Text{
            text:"test"
        }
    }
}
