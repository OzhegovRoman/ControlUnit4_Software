import QtQuick 2.4
import QtQuick.Controls 2.4

import AppCore 1.0

SspdForm {

    property double dataPixelPart:  0.4
    property int buttonMargin:      (Qt.platform.os =="windows") ? 0    : dp(2)
    property int pixelSize:         (Qt.platform.os =="windows") ? 24   : dp(24)


    property double sspdCurrent:            0
    property double sspdVoltage:            0
    property double sspdCounts:             0
    property double sspdAutoResetThreshold: 0
    property double sspdAutoResetTimeOut:   0
    property double sspdCmpRefLevel:        0
    property double sspdCounterTimeConst:   0
    property int sspdShorted:               0
    property int sspdAmplifierTurnedOn:     0
    property int sspdAutoResetTurnedOn:     0
    property int sspdComparatorTurnedOn:    0
    property int sspdCounterTurnedOn:       0

    property int lDriverAddress;

    function updateSspdData(){
        sspdCurrent = AppCore.sspdCurrent*1E6;
        sspdVoltage = AppCore.sspdVoltage*1E3;
        sspdCounts  = AppCore.sspdCounts;
        sspdShorted = AppCore.sspdShorted                           ? 1 : 0;
        sspdAmplifierTurnedOn   = AppCore.sspdAmplifierTurnedOn     ? 1 : 0;
        sspdAutoResetTurnedOn   = AppCore.sspdAutoResetTurnedOn     ? 1 : 0;
        sspdComparatorTurnedOn  = AppCore.sspdComparatorTurnedOn    ? 1 : 0;
        console.log(sspdComparatorTurnedOn);
        for (var i = 0; i<settingsModel.rowCount(); i++){
            switch (settingsModel.get(i)["name"]){
            case "current":
                settingsModel.setProperty(i, "value", sspdCurrent);
                break;
            case "voltage":
                settingsModel.setProperty(i, "value", sspdVoltage);
                break;
            case "short":
                settingsModel.setProperty(i, "value", sspdShorted);
                break;
            case "amplifier":
                settingsModel.setProperty(i, "value", sspdAmplifierTurnedOn);
                break;
            case "cmp_on":
                settingsModel.setProperty(i, "value", sspdComparatorTurnedOn);
                break;
            case "counter":
                if (sspdComparatorTurnedOn === 1)
                    settingsModel.setProperty(i, "value", sspdCounts);
                else
                    settingsModel.setProperty(i, "value", 0);
                break;
            case "autoreset_on":
                settingsModel.setProperty(i, "value", sspdAutoResetTurnedOn);
                break;
            }
        }
    }

    function updateSspdParams(){
        sspdAutoResetThreshold  = AppCore.sspdAutoResetThreshold;
        sspdAutoResetTimeOut    = AppCore.sspdAutoResetTimeOut;
        sspdCmpRefLevel         = AppCore.sspdCmpRefLevel*1E3;
        sspdCounterTimeConst    = AppCore.sspdCounterTimeConst;
    }

    function setData(fName, fText) {
        switch (fName){
        case "current":
            AppCore.sspdCurrent = fText*1e-6;
            break;
        case "cmp":
            AppCore.sspdCmpRefLevel = fText*1e-3
            break;
        case "counter_timeOut":
            AppCore.sspdCounterTimeConst = fText
            break;
        case "threshold":
            AppCore.sspdAutoResetThreshold = fText
            break;
        case "timeOut":
            AppCore.sspdAutoResetTimeOut = fText
            break;
        }
    }

    Timer {
        id: timer
        interval: 500;
        running: true;
        repeat: true;
        onTriggered: {
            AppCore.getSspdData(lDriverAddress);
        }
    }
    Component.onCompleted:{
        lDriverAddress = currentDriverAddress;
        AppCore.initializeSspdDriver(lDriverAddress);
    }
    Connections {
        target: AppCore
        onSspdDriverInitialized: {
            updateSspdData();
            updateSspdParams();
        }
        onSspdDataUpdated: {
            updateSspdData();
        }
    }

    property int timerCounts: 0;
    property int direction: 1;
    property int modelIndex: 0;

    function startClickTimer(){
        timer.stop();
        clickTimer.interval = 1000;
        timerCounts = 0;
        triggerClickTimer();
        clickTimer.start();
    }

    function triggerClickTimer(){
        timerCounts++;
        if (timerCounts == 3) clickTimer.interval = 200;
        if (timerCounts == 20) clickTimer.interval = 100;
        settingsModel.get(modelIndex)["value"] += direction*settingsModel.get(modelIndex)["step"];
        //value += direction * step;
        //        console.log("tick "+timerCounts);
    }

    Timer {
        id: clickTimer
        running: false;
        repeat: true;
        onTriggered: {
            triggerClickTimer()
        }
    }


    SetttingsModel {
        id: settingsModel
    }
    listView {
        model: settingsModel
        delegate: Item {
            height: (pixelSize*1.5<30) ? 40 : pixelSize*1.5
            width: parent.width


            Text {
                text: model.itemText
                font.pixelSize: pixelSize
                anchors.verticalCenter: parent.verticalCenter
            }

            Item {
                height: dp(30)
                anchors.right: parent.right
                width: parent.width * dataPixelPart
                anchors.verticalCenter: parent.verticalCenter

                visible: model.type == "changable"

                RoundButton {
                    id: plusButton
                    radius: dp(16)
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
                    onPressed: {
                        direction = 1
                        startClickTimer();
                        modelIndex = model.index;
                    }
                    onReleased:{
                        clickTimer.stop();
                        setData(model.name, textInput.text);
                        timer.start();
                    }
                }

                TextInput {
                    id: textInput
                    text: parseFloat(model.value).toFixed(model.fixed)
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: plusButton.left
                    anchors.rightMargin: buttonMargin
                    anchors.left: minusButton.right
                    anchors.leftMargin: buttonMargin
                    font.pixelSize: pixelSize
                    horizontalAlignment: Text.AlignHCenter
                    inputMethodHints: Qt.ImhDigitsOnly
                    validator: RegExpValidator {
                        regExp: /[-+]?[0-9]*\.?[0-9]*/
                    }
                    onFocusChanged: {
                        if (textInput.focus == true){
                            timer.stop();
                        }
                    }
                    onEditingFinished: {
                        setData(model.name, textInput.text);
                        textInput.focus = false;
                        timer.start();
                    }

                }
                RoundButton {
                    id: minusButton
                    radius: 16
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
                    onPressed: {
                        direction = -1
                        startClickTimer();
                        modelIndex = model.index;
                    }
                    onReleased:{
                        clickTimer.stop();
                        setData(model.name, textInput.text);
                        timer.start();
                    }
                }

            }

            Text {
                visible: model.type == "unchangable"
                text: parseFloat(model.value).toFixed(model.fixed)
                font.pixelSize: pixelSize
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * dataPixelPart
                horizontalAlignment: Text.AlignHCenter
            }

            Switch {
                visible: type == "switch"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * dataPixelPart
                checked: model.value
                onToggled: {
                    switch (model.name){
                    case "short":
                        AppCore.sspdShorted = checked;
                        break;
                    case "amplifier":
                        AppCore.sspdAmplifierTurnedOn = checked;
                        break;
                    case "cmp_on":
                        AppCore.sspdComparatorTurnedOn = checked;
                        break;
                    case "autoreset_on":
                        AppCore.sspdAutoResetTurnedOn = checked;
                        break;
                    }
                }
            }
        }
    }
}
