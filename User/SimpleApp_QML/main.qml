import QtQuick 2.11
import QtQuick.Controls 2.0
import QtQuick.Window 2.11

import AppCore 1.0

ApplicationWindow {
    visible: true
    width: 400
    height: 600

    readonly property int dpi: Screen.pixelDensity * 25.4 * Screen.devicePixelRatio

    AppCore {
        id: appcore
        devList: deviceList
        mUnitData: unitData

        onConnectionReject: {
            rootItem.state = "tcpIpConnectScreen"
            console.log("Connection rejected. lastTcpIpAddress: " + lastIpAddress);
        }
        onConnectionApply: {
            //хрен знает но без вывода почему то не работает
            rootItem.state = "workScreen"
            console.log("Connection accepted. TcpIpAddress: " + lastIpAddress);
        }
    }

    function dp(x){ return (dpi < 480) ? x : x * (dpi/480); }

    function startUp(){
        console.log("startUp function");
        console.log(dpi);
        console.log("PixelDensity:"+Screen.pixelDensity);
        console.log("PixelRatio:"+Screen.devicePixelRatio);
        console.log("10:"+ dp(10));
        console.log("100:"+ dp(100));
        appcore.coreConnectToDefaultIpAddress();
    }

    function connectToIpAddress(address){
        console.log("Connect to TcpIp adress "+ address);
        rootItem.state = "welcomeScreen"
        appcore.coreConnectToIpAddress(address);
    }

    function reconnect(){
        console.log("reconnected")
        rootItem.state = "tcpIpConnectScreen"
    }

    Item {
        id: rootItem
        anchors.fill: parent

        state: "welcomeScreen"

        Loader {
            id: pageLoader
            anchors.fill: parent
            source: "WelcomeForm.qml"
            Component.onCompleted: startUp()
        }

        states: [
            State {
                name: "welcomeScreen"
                PropertyChanges {
                    target: pageLoader;
                    source: "WelcomeForm.qml"
                }
            },
            State {
                name: "tcpIpConnectScreen"
                PropertyChanges {
                    target: pageLoader;
                    restoreEntryValues: false
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
}
