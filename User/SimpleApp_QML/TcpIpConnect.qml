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

    Component.onCompleted:
        tcpIpAddressText.text = appcore.lastIpAddress

    signal clicked;

    onClicked: {
        console.log("connect");
        connectToIpAddress(tcpIpAddress);
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
}
