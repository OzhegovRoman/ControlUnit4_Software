import QtQuick 2.4
import QtQuick.Controls 2.3

Item {
    id: uiTemperatureForm
    anchors.fill: parent
    state: "short"

    property alias temperatureText: temperatureText
    property alias pressureText: pressureText
    property alias tempSensorVoltageText: tempSensorVoltageText
    property alias pressSensorVoltageText: pressSensorVoltageText
    property alias smallTextSize: smallTextSize
    property alias mediumTextSize: mediumTextSize
    property alias largeTextSize: largeTextSize
    property alias marginSize: marginSize

    property int smallTextSize: 18
    property int largeTextSize: 40
    property int mediumTextSize: 24
    property int marginSize: 10

    Button {
        id: button
        text: (uiTemperatureForm.state === "full") ? qsTr("Less") : qsTr("More")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: mediumTextSize
        onClicked: parent.state = parent.state == "short" ? "full" : "short"
    }

    Text {
        id: temperatureText
        text: qsTr("T: ---- K")
        font.pixelSize: largeTextSize
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: button.top
        anchors.bottomMargin: marginSize
    }

    Rectangle {
        id: rectangle1

        visible: uiTemperatureForm.state === "full"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: button.bottom
        anchors.topMargin: marginSize

        width: childrenRect.width

        Column {
            id: column
            Text {
                id: pressureText
                text: qsTr("P: --- mbar")
                font.pixelSize: smallTextSize
            }

            Text {
                id: tempSensorVoltageText
                text: qsTr("Temp sensor voltage: ----- V")
                font.pixelSize: smallTextSize
            }

            Text {
                id: pressSensorVoltageText
                text: qsTr("Press sensor Voltage: --- V")
                font.pixelSize: smallTextSize
            }
        }
    }

    states: [
        State {
            name: "full"
        },
        State {
            name: "short"
        }
    ]
}
