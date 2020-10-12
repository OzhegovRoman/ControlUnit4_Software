import QtQuick 2.11

Item {
    id: root

    width: 24
    height: 24
    
    property real value: 0 //from 0 to 1
    
    property color color: "black"
    property color lineColor: "white"
    property color borderColor: "green"

    signal clicked();

    Rectangle {
        property real size: (parent.width<parent.height) ? parent.width : parent.height
        width: size
        height: size
        anchors.centerIn: parent
        color: parent.color
        border.color: parent.borderColor

        Item {
            rotation: root.state == "menu" ? value * 180 : -value * 180
            anchors{
                fill: parent
                margins: 0.2*parent.size
            }

            Rectangle {
                id: bar1
                x: 0.4 * value * parent.width
                y: (0.125 + 0.135 * value) * parent.height
                width:  (1 - 0.4 * value) * parent.width
                height: 0.15 * parent.height
                rotation: 45 * value
                antialiasing: true
                color: root.lineColor
            }

            Rectangle {
                id: bar2
                x: 0.1 * value * parent.width
                y: 0.425* parent.height
                width: (1-0.2*value) * parent.width
                height: 0.15*parent.height
                antialiasing: true
                color: root.lineColor
            }
            Rectangle {
                id: bar3
                x: 0.4 * value * parent.width
                y: (0.725 - 0.135 * value) * parent.height
                width:  (1 - 0.4 * value) * parent.width
                height: 0.15*parent.height
                rotation: -45 * value
                antialiasing: true
                color: root.lineColor
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.clicked()
        }
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
        if (value == 1) root.state = "back"
        else if(value == 0) root.state = "menu"
    }
}
