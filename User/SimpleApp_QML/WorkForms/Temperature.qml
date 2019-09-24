import QtQuick 2.4

import AppCore 1.0

TemperatureForm {

    property int lDriverAddress;

    smallTextSize: dp(18)
    largeTextSize: dp(40)
    mediumTextSize: dp(24)
    marginSize: dp(10)

    function connectSensor(){
        lDriverAddress = currentDriverAddress;
        AppCore.connectTemperatureSensor(currentDriverAddress);
    }

    function disConnectSensor(){
        AppCore.disConnectTemperatureSensor(lDriverAddress);
    }

    Component.onCompleted: {
        connectSensor();
    }

    Component.onDestruction: {
        disConnectSensor();
    }

    Timer {
        interval: 500;
        running: true;
        repeat: true;
        onTriggered: {
            AppCore.getTemperatureDriverData(lDriverAddress);
        }
    }

    Connections {
        target: AppCore
        onTemperatureDataDone:  {
            if (AppCore.temperature == 0)
                temperatureText.text = "N/C";
            else
                temperatureText.text = "T: " + parseFloat(AppCore.temperature).toFixed(1)+" K";
            pressureText.text = "P: " + parseFloat(AppCore.pressure).toFixed(1) + " mbar";
            tempSensorVoltageText.text = "Temp sensor voltage: " + parseFloat(AppCore.tempSensorVoltage).toFixed(3)+" V";
            pressSensorVoltageText.text = "Press sensor voltage: " + parseFloat((AppCore.pressSensorVoltageP - AppCore.pressSensorVoltageN)*1000).toFixed(2)+ " mV";
            if (AppCore.commutatorOn === false) connectSensor();
        }
    }
}
