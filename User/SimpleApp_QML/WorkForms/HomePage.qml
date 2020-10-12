import QtQuick 2.11

Item {
    id: uiHomePageForm
    anchors.fill: parent
    property int fontSize: dp(24)

    Text {
        id: text1
        width: parent.width * 0.6
        wrapMode: Text.WordWrap
        text: qsTr("Please choose a device driver from the side menu...")
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: fontSize
    }
}
