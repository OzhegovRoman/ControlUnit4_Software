import QtQuick 2.4

import TcpIpValidator 1.0

TcpIpConnectForm {

    normalTextSize: dp(24)
    smallTextSize: dp(18)

    TcpIpValidator{
        id: tcpIpValidator
        onValidateChanged: {
            tcpIpAddressText.color = validate ? "black" : "red";
            buttonConnect.enabled = validate;
        }
    }

    function connect(){
        connectionToIpAddress(tcpIpAddressText.text);
    }

    buttonConnect {
        onClicked: connect()
    }

    tcpIpAddressText {
        text: lastTcpIpAddress
        //inputMethodHints: Qt.ImhNoPredictiveText
        onTextChanged: {
            tcpIpValidator.textToValidate = tcpIpAddressText.text
        }
        onAccepted: connect()
    }
}
