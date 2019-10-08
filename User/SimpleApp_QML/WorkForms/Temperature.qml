import QtQuick 2.13
import QtQuick.Controls 2.13

import AppCore 1.0

Item {

    id: temperatureForm
    anchors.fill: parent
    state: "short"

    property int smallTextSize: dp(18)
    property int largeTextSize: dp(40)
    property int mediumTextSize: dp(24)
    property int marginSize: dp(10)

    property int lDriverAddress: appcore.currentDriverAddress;

    states: [
        State {name: "full"},
        State {name: "short"}
    ]

        function connectSensor(){
            lDriverAddress = currentDriverAddress;
            appcore.connectTemperatureSensor(currentDriverAddress, true);
        }

        Component.onCompleted: {
            connectSensor();
        }

        Component.onDestruction: {
            if (appcore)
                appcore.connectTemperatureSensor(lDriverAddress, false);
        }

    Button {
        id: button
        text: (parent.state === "full") ? qsTr("Less") : qsTr("More")
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        font.pixelSize: mediumTextSize
        onClicked: parent.state = parent.state === "short" ? "full" : "short"
    }

    Text {
        id: temperatureText
        text: "T: " + (temperatureData.temperature < 0.1 ? "N/C" : (parseFloat(temperatureData.temperature).toFixed(1) + " K"))
        font.pixelSize: largeTextSize
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: button.top
        anchors.bottomMargin: marginSize
    }

    Column {
        id: column
        visible: temperatureForm.state === "full"
        anchors{
            horizontalCenter: parent.horizontalCenter
            top: button.bottom
            topMargin: marginSize
        }

        Text {
            id: tempSensorVoltageText
            text: qsTr("Temp sensor voltage: " + parseFloat(temperatureData.temperatureSensorVoltage).toFixed(3) + " V")
            font.pixelSize: smallTextSize
        }

        Text {
            id: pressureText
            text: "P: " + parseFloat(temperatureData.pressure).toFixed(1) + " mbar"
            font.pixelSize: smallTextSize
        }

        Text {
            id: pressSensorVoltageText
            text: qsTr("Press sensor Voltage: ") + parseFloat(temperatureData.pressureSensorVoltage*1000.0).toFixed(0)+ " mV"
            font.pixelSize: smallTextSize
        }
    }

    Timer {
        interval: 500;
        running: true;
        repeat: true;
        onTriggered: {
            if (temperatureData.isConnected === false) connectSensor();
            appcore.getTemperatureDriverData(lDriverAddress);
        }
    }
}
