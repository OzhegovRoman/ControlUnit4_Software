import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0

Item {
    id: uiWelcomeForm
    anchors.fill: parent

    property int fontSize: 24

    Text {
        id: text1
        width: parent.width * 0.6
        wrapMode: Text.WordWrap
        text: qsTr("Please wait while the connecttion with ControlUnit is established...")
        horizontalAlignment: Text.AlignHCenter
        anchors.bottom: busyIndicator.top
        anchors.bottomMargin: fontSize * 0.5
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: fontSize
    }

    BusyIndicator {
        id: busyIndicator
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height * 0.4
        width: parent.width * 0.4
    }
}
