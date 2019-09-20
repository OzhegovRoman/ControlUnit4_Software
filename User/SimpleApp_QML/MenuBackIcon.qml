import QtQuick 2.0

Item {
    id: root
    width: dp(24)
    height: dp(24)

    property real value: 0 //from 0 to 1
    rotation: root.state == "menu" ? value*180 : -value*180


    Rectangle {
        id: bar1
        x: dp(2 + 7.5*value)
        y: dp(5 + 3*value)
        width: dp(20 - 7*value)
        height: dp(2)
        rotation: 45*value
        antialiasing: true
    }

    Rectangle {
        id: bar2
        x: dp(2 + value)
        y: dp(10 + 2*value)
        width: dp(20 - 3*value)
        height: dp(2)
        antialiasing: true
    }

    Rectangle {
        id: bar3
        x: dp(2 + 7.5*value)
        y: dp(15 + value)
        width: dp(20 - 7*value)
        height: dp(2)
        rotation: -45*value
        antialiasing: true
    }

    state: "menu"
    states: [
        State {
            name: "menu"
        },

        State {
            name: "back"
        }
    ]

    onValueChanged: {
            if(value == 1) root.state = "back"
            else if(value == 0) root.state = "menu"
        }
}
