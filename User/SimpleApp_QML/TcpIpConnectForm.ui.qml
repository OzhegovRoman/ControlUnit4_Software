import QtQuick 2.4
import QtQuick.Controls 2.3

import TcpIpValidator 1.0

Item {
    id: uiTcpIpConnectForm
    anchors.fill: parent

    property alias tcpIpAddressText: tcpIpAddressText
    property alias buttonConnect: buttonConnect

    property int normalTextSize: 24
    property int smallTextSize: 18

    TextField {
        id: tcpIpAddressText
        width: parent.width * 0.8
        font.underline: false
        font.pixelSize: normalTextSize
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        placeholderText: qsTr("000.000.000.000")
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: text1
        text: qsTr("Please enter IP-address:")
        anchors.bottom: tcpIpAddressText.top
        font.pixelSize: smallTextSize
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: tcpIpAddressText.horizontalCenter
    }

    Button {
        id: buttonConnect
        text: qsTr("Connect")
        autoRepeat: false
        font.pixelSize: normalTextSize
        anchors.top: tcpIpAddressText.bottom
        anchors.topMargin: smallTextSize * 0.5
        anchors.horizontalCenter: tcpIpAddressText.horizontalCenter
    }
}
