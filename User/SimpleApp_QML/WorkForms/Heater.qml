import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
    id: heaterForm
    property int pixelSize: dp(24)

    anchors.fill: parent
    Rectangle{
        id: _container
        anchors.fill: parent
        border.color: "black"
    }

    Text {
        id: name
        text: qsTr("heater form")
    }

    HeaterButton {
        width: heaterForm.width * 0.8
        height:  heaterForm.pixelSize * 3
        pixelSize: heaterForm.pixelSize
        anchors.horizontalCenter: heaterForm.horizontalCenter
        anchors.top: heaterForm.top
        anchors.topMargin: 100
    }

}
