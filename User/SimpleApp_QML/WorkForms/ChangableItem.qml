import QtQuick 2.11
import QtQuick.Controls 2.0
import QtQml 2.11

Item {
    id: changableItem
    signal changed (real newValue);

    property double value: 0
    property int fixed: 1
    property double step: 0.1
    property int size: 32
    property int timerCounts: 0
    property int direction: 1
    property double tmpValue: 0
    property bool editing: false;


    height: size

    function startTimer(dir){
        console.log("startTimer");
        direction = dir
        clickerTimer.interval = 1000;
        timerCounts = 0;
        tmpValue = value;
        triggerTimer();
        clickerTimer.start();
    }

    function triggerTimer(){
        timerCounts++;
        if (timerCounts == 2) clickerTimer.interval = 200;
        if (timerCounts == 20) clickerTimer.interval = 100;
        tmpValue += direction * step;
    }

    function stopTimer(){
        if (clickerTimer.running)
        {
            console.log("stop. "+tmpValue);
            changed(tmpValue);
            clickerTimer.stop();
        }
    }

    Item {
        id: plusButton
        width: size
        height: size
        clip: true

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        Image{
            source: "../png/add.png"
            anchors.fill: parent
        }

        MouseArea{
            anchors.fill: parent;
            onPressed:{
                startTimer(1);
                plusButton.focus = true
            }
            onReleased: {
                stopTimer();
            }
            onCanceled:{
                stopTimer();
            }
            onExited:{
                stopTimer();
            }
        }
    }

    Item {
        id: minusButton
        width: size
        height: size
        clip: true

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }

        Image{
            source: "../png/delete.png"
            anchors.fill: parent
        }

        MouseArea{
            anchors.fill: parent;
            onPressed: {
                startTimer(-1);
                minusButton.focus = true;
            }
            onReleased: {
                console.log(tmpValue);
                stopTimer();
            }
            onCanceled:{
                console.log(tmpValue);
                stopTimer();
            }
            onExited:{
                console.log(tmpValue);
                stopTimer();
            }
        }
    }

    TextInput {
        id: textInput
        property string tempStr;
        text: activeFocus ? tempStr: parseFloat(clickerTimer.running ? tmpValue : value).toFixed(fixed)
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhPreferNumbers
        anchors{
            verticalCenter: parent.verticalCenter
            right: plusButton.left
            left: minusButton.right
        }
        font.pixelSize: 0.75 * parent.size
        horizontalAlignment: Text.AlignHCenter
        validator: RegExpValidator {
            regExp: /[-+]?[0-9]*\.?[0-9]*/
        }
        Keys.onReturnPressed:{
            console.log("onReturnPressed")
            changableItem.changed(displayText);
            focus = false;
        }
        Keys.onEnterPressed: {
            console.log("onEnterPressed")
            changableItem.changed(displayText);
            focus = false;
        }
        Keys.onEscapePressed:{
            console.log("onEscapePressed")
            focus = false;
        }
        onAccepted: {
            console.log("onAccepted");
        }
        onEditingFinished: {
            console.log("onEditingFinished. Value: "+value);
        }
        onActiveFocusChanged: {
            if (activeFocus){
                tempStr = displayText;
                selectAll()
            }
            console.log("onFocusChanged "+focus)
        }
    }

    Timer {
        id: clickerTimer
        running: false
        repeat: true
        onTriggered: {
            triggerTimer();
        }
    }
}

