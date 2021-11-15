import QtQuick 2.0
import QtQuick.Controls 2.12

import AppCore 1.0

Item {
    id: heaterForm
    property int pixelSize: dp(24)

    property double dataPart: 0.4

    property int lDriverAddress: currentDriverAddress;
    property string lDriverType: currentDriverType;
    property int timerTicks: 0
    property double rearEdgeTime: 0.0
    property double holdTime: 0.0
    property double frontEdgeTime: 0.0

    function updateTimesFromModel(){
        heaterForm.frontEdgeTime    =  model_1.data.value("frontedge") *1000.0;
        heaterForm.holdTime         =  model_1.data.value("holdtime")  *1000.0;
        heaterForm.rearEdgeTime     =  model_1.data.value("rearedge")  *1000.0;

    }

    Component.onCompleted: {
        //также добавляем и получение параметров. которые в принципе получать постоянно не надо
        // параметры получаем раз в 5 секунд, чаще не надо
        appcore.prepareUnitData(lDriverAddress, lDriverType);
        model_1.modelReset();
        appcore.updateDriverData(lDriverAddress, lDriverType);

        updateTimesFromModel();
    }

    anchors.fill: parent

    ScrollBar {
        id: scrollview
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: _heaterButton.top
        anchors.bottomMargin: _heaterButton.height
        clip: true

        ListView {
            id: listview
            anchors {
                rightMargin: parent.width * 0.02
                leftMargin: parent.width * 0.02
                bottomMargin: parent.height * 0.01
                topMargin: parent.height * 0.01
                fill: parent
            }

            spacing: pixelSize * 0.2

            model: UnitDataModel{
                id: model_1
                data: unitData
            }

            section {
                property: "group"
                criteria: ViewSection.FullString
                delegate: Rectangle {
                    width: listview.width
                    height: childrenRect.height
                    color: "transparent"
                    Text {
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.bold: true
                        text: section
                        font.pixelSize: pixelSize * 1.2
                    }
                }
            }

            delegate: Item {
                id : delegateItem
                height: (pixelSize*1.5<30) ? 40 : pixelSize*1.5
                width: parent.width
                Text {
                    text: model.title
                    font.pixelSize: pixelSize
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                }
                ChangableItem {
                    id: changableItem
                    visible: model.type === "changable"
                    size: dp(32)
                    width: parent.width * dataPart
                    value: model.value
                    fixed: model.fixed
                    step: model.step
                    anchors{
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    onChanged: {
                        model.value = newValue
                        updateTimesFromModel();
                    }
                }

                Text {
                    id: unchangableItem
                    visible: model.type === "unchangable"
                    text: parseFloat(model.value).toFixed(model.fixed)
                    font.pixelSize: pixelSize
                    width: parent.width * dataPart
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    horizontalAlignment: Text.AlignHCenter
                }

                Switch {
                    id: switchItem
                    visible: type === "switch"
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    width: parent.width * dataPart
                    checked: model.value
                    onToggled:
                        model.value = checked ? 1 : 0
                }
            }
        }
    }

    HeaterButton {
        id: _heaterButton
        width: heaterForm.width * 0.8
        height:  heaterForm.pixelSize * 3
        pixelSize: heaterForm.pixelSize
        rearEdgeTime: heaterForm.rearEdgeTime
        waitingTime: heaterForm.frontEdgeTime + heaterForm.holdTime + heaterForm.rearEdgeTime
        anchors.horizontalCenter: heaterForm.horizontalCenter
        anchors.bottom: heaterForm.bottom
        anchors.bottomMargin: height

        onStartHeating: {
            appcore.startHeating(lDriverAddress, lDriverType)
            console.log()
        }
        onEmergencyStop: {
            console.log("Time:"+ _heaterButton.waitingTime)
            appcore.emergencyStop(lDriverAddress, lDriverType)
        }
    }
}
