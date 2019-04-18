import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.0
import QtQuick.Window 2.0

import AppCore 1.0

ApplicationWindow {
    width: 400
    height: 600
    visible: true
    Material.theme: Material.Light
    Material.accent: Material.Green

    // Conversion independent of the density of pixels to physical pixels the device
    readonly property int dpi: Screen.pixelDensity * 25.4
    function dp(x){ return (dpi < 120) ? x : x * (dpi/160); }
    function startUp(){
        console.log(dpi);
        AppCore.coreConnectToDefaultIpAddress();
    }

    function connectionToIpAddress(address){
        mainProcess.state = "welcomeScreen"
        AppCore.coreConnectToIpAddress(address);
    }

    property string lastTcpIpAddress: "127.0.0.1"

    Connections {
        target: AppCore
        onConnectionReject: {
            mainProcess.state = "tcpIpConnectScreen"
            lastTcpIpAddress = AppCore.coreMessage
            console.log("lastTcpIpAddress: "+AppCore.coreMessage);
        }
        onConnectionApply: function() {
            console.log(AppCore.coreMessage);
            var tmpStr = AppCore.coreMessage;
            tmpStr = tmpStr.replace(/\r?\n|\r/g,"");
            var driverList = tmpStr.split(";<br>");
            driverList.splice(0,1);

            devModel.clear();
            var count = 0;
            for (var i = 0; i< driverList.length; i++){
                var tmp = driverList[i];
                var type;
                if (tmp.search(/CU4SD/)>=0) type = "SSPD Driver"
                if (tmp.search(/CU4TD/)>=0) type = "Temperature"

                var address;
                address = tmp.match(/address=[0-9]{1,2}:/g)[0].replace(/address=/,"").replace(":","");
                devModel.append({
                                    "type": type,
                                    "address": address
                                });
            }
            mainProcess.state = "workScreen"
        }
    }

    Item {
        id: mainProcess
        Component.onCompleted: startUp();
        anchors.fill: parent

        state: "welcomeScreen"

        Loader {
            id: pageLoader
            anchors.fill: parent
        }

        states: [
            State {
                name: "welcomeScreen"
                PropertyChanges {
                    target: pageLoader;
                    source: "WelcomeForm.ui.qml"
                }
            },
            State {
                name: "tcpIpConnectScreen"
                PropertyChanges {
                    target: pageLoader;
                    source: "TcpIpConnect.qml"
                }
            },
            State {
                name: "workScreen"
                PropertyChanges {
                    target: pageLoader;
                    source: "Work.qml"
                }
            }
        ]
    }
    ListModel {
        id: devModel
    }
}
