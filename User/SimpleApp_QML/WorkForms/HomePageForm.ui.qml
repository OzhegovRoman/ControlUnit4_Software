import QtQuick 2.4

Item {
    id: uiHomePageForm
    anchors.fill: parent

    //clip: true
    Text {
        id: text1
        width: parent.width * 0.6
        wrapMode: Text.WordWrap
        text: qsTr("Please choose device driver from side menu...")
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: dp(24)
    }
}
