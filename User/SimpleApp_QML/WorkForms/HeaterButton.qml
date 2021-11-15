import QtQuick 2.0
import QtQml 2.13

Item{
    id: heaterButton
    property int pixelSize: 24
    property int waitingTime: 5000
    property int rearEdgeTime: 1000

    signal startHeating();
    signal emergencyStop();
    signal stopHeated();

    property double startHeatingTime: 0.0

    Timer {
        id: heaterTimer
        interval: 10
        running: false
        repeat: true
        onTriggered: {
            progress.heatingValue = (new Date().getTime() - startHeatingTime) / waitingTime * 100.0
            if (progress.heatingValue >= 100){
                stop();
                progressText.text = "Hold button to Heating...";
                progress.heatingValue = 0
                stopHeated()
            }
        }
    }

    Rectangle{
        id: progress
        property int value: 0
        property int heatingValue: 0

        color: pressAnimation.running ? "#FF4A11" : "white"
        border.color: "grey"
        border.width: 2
        clip: true
        radius: 5

        NumberAnimation on value {
            id: pressAnimation
            running: false
            from: 0; to: 100
            duration: 500

            onStopped: {
                if (progress.value === 100){
                    if (heaterTimer.running){
                        startHeatingTime = new Date().getTime() - waitingTime + rearEdgeTime
                        progressText.text = "Wait for stoping..."

                        emergencyStop();
                    }
                    else
                    {
                        progressText.text = "Heating. Hold to stop..."

                        startHeating();
                        startHeatingTime = new Date().getTime()
                        heaterTimer.start();

                        console.log("waitingTime", waitingTime)
                        console.log("start", new Date().toISOString());
                    }

                }
                progress.value = 0
            }
        }
        MouseArea {
            id: myMouse
            anchors.fill: progress
            onPressed: {
                pressAnimation.start();
            }
            onReleased: {
                pressAnimation.stop();
            }

        }

        Rectangle{
            anchors.left: progress.left
            anchors.top: progress.top
            anchors.bottom: progress.bottom
            border.color: "grey"
            border.width: 2
            radius: 5
            color: pressAnimation.running ? "#FF8411" : "#39BFEF"
            width: progress.width / 100  * (pressAnimation.running ? progress.value : progress.heatingValue)
        }

        Text{
            id: progressText
            text: "Hold button to Heating..."
            wrapMode: Text.WordWrap
            anchors.fill: progress
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "black"
            font.pixelSize: heaterForm.pixelSize
        }

        anchors.fill: parent
    }
}
