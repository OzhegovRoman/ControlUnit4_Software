import QtQuick 2.4

TcpIpConnectForm {
    function connect(){
        // проверяем строку на правильность ввода
        if (tcpIpAddressText.acceptableInput){
            connectionToIpAddress(tcpIpAddressText.text);
        }
    }

    buttonConnect {
        onClicked: connect()
    }

    tcpIpAddressText {
        text: lastTcpIpAddress
        inputMethodHints: Qt.ImhNoPredictiveText
        onAccepted: connect()
        onTextChanged: {
            if (tcpIpAddressText.acceptableInput)
                tcpIpAddressText.color = "black"
            else
                tcpIpAddressText.color = "red"
        }
    }
}
