import QtQuick 2.4
import QtQuick.Controls 2.3

Item {
    id: uiTcpIpConnectForm
    anchors.fill: parent

    property alias tcpIpAddressText: tcpIpAddressText
    property alias buttonConnect: buttonConnect
    Text {
        id: text1
        text: qsTr("Please enter Ip-adress")
        anchors.top: parent.top
        anchors.topMargin: 40
        font.pixelSize: 18
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    TextField {
        id: tcpIpAddressText
        anchors.top: text1.bottom
        anchors.topMargin: 0
        font.underline: false
        font.pixelSize: 24
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        placeholderText: qsTr("000.000.000.000")
        validator: RegExpValidator {
            regExp: /^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/
        }
    }

    Button {
        id: buttonConnect
        text: qsTr("Connect")
        anchors.topMargin: 0
        anchors.top: tcpIpAddressText.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
