import QtQuick 2.4
import QtQuick.Controls 2.4

Item {
    id: item1
    property string type: "changable"
    property string text: "----"
    property string label
    property alias plusButton: plusButton
    property alias minusButton: minusButton
    property alias textInput: textInput
    height: dp(24) //30

    visible: type == "changable"

    RoundButton {
        id: plusButton
        radius: dp(12)
        anchors.right: parent.right
        anchors.leftMargin: 0
        anchors.verticalCenter: parent.verticalCenter
        hoverEnabled: true
        visible: true
        padding: 0
        Image {
            id: imPLus
            source: "../png/add.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    TextInput {
        id: textInput
        text: parent.text
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: plusButton.left
        anchors.rightMargin: 0
        anchors.left: minusButton.right
        anchors.leftMargin: 0
        font.pixelSize: dp(24)
        horizontalAlignment: Text.AlignHCenter
        validator: RegExpValidator {
            regExp: /[-+]?[0-9]*\.?[0-9]*/
        }
    }

    RoundButton {
        id: minusButton
        radius: dp(12)
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.verticalCenter: parent.verticalCenter
        hoverEnabled: true
        visible: true
        padding: 0
        Image {
            id: imMinus
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            source: "../png/delete.png"
        }
    }
}
