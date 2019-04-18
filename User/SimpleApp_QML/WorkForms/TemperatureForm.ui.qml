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

    Button {
        id: button
        text: (uiTemperatureForm.state === "full") ? qsTr("Less") : qsTr("More")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        onClicked: parent.state = parent.state == "short" ? "full" : "short"
    }

    Text {
        id: temperatureText
        text: qsTr("T: ---- K")
        font.pixelSize: 40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: button.top
        anchors.bottomMargin: 10
    }

    Rectangle {
        id: rectangle1

        visible: uiTemperatureForm.state === "full"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: button.bottom
        anchors.topMargin: 10

        width: childrenRect.width

        Column {
            id: column
            Text {
                id: pressureText
                text: qsTr("P: --- mbar")
                font.pixelSize: 18
            }

            Text {
                id: tempSensorVoltageText
                text: qsTr("Temp sensor voltage: ----- V")
                font.pixelSize: 18
            }

            Text {
                id: pressSensorVoltageText
                text: qsTr("Press sensor Voltage: --- V")
                font.pixelSize: 18
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

