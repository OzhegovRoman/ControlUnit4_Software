import QtQuick 2.4
import QtQuick.Controls 2.4

ChangableItemForm {
    signal changed;
    signal blocked;

    property double value: 0
    property int fixed: 0
    property double step: 0.1
    property int timerCounts: 0
    property int direction: 1

    height: dp(24)

    function startTimer(){
        blocked();
        clickTimer.interval = 1000;
        timerCounts = 0;
        triggerTimer();
        clickTimer.start();
    }

    function triggerTimer(){
        timerCounts++;
        if (timerCounts == 2) clickTimer.interval = 200;
        if (timerCounts == 20) clickTimer.interval = 100;
        value += direction * step;
    }

    Timer {
        id: clickTimer
        running: false;
        repeat: true;
        onTriggered: {
            triggerTimer()
        }
    }

    text: parseFloat(value).toFixed(fixed)

    textInput.onEditingFinished: function(){
        console.log("changed");
        textInput.focus = false;
        changed();
    }
    textInput.onFocusChanged: function(){
        if (textInput.focus === true){
            blocked();
            console.log("blocked");
        }
    }

    plusButton.onPressed: function(){
        direction = 1;
        startTimer();
    }
    plusButton.onReleased: function(){
        clickTimer.stop();
        changed();
    }
    minusButton.onPressed: function(){
        direction = -1;
        startTimer();
    }
    minusButton.onReleased: function(){
        clickTimer.stop();
        changed();
    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
