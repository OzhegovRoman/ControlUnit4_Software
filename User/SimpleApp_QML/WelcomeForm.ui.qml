import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0

Item {
    id: uiWelcomeForm
    anchors.fill: parent

    //clip: true
    Text {
        id: text1
        width: parent.width * 0.6
        wrapMode: Text.WordWrap
        text: qsTr("Please wait while the connecttion with ControlUnit is established...")
        horizontalAlignment: Text.AlignHCenter
        anchors.bottom: busyIndicator.top
        anchors.bottomMargin: dp(24)
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: dp(24)
    }

    BusyIndicator {
        id: busyIndicator
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        height: dp(120)
        width: dp(120)
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
