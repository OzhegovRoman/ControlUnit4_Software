import QtQuick 2.0

Item{
    id: heaterButton
    property int pixelSize: 24
    property int waitingTime: 5000
    property int rearEdgeTime: 1000

    signal startHeating();
    signal emergencyStop();
    signal stopHeated();

    Rectangle{
        id: progress
        property int value: 0
        property int heatingValue: 0

        color: pressAnimation.running ? "#FF4A11" : "white"
        border.color: "grey"
        border.width: 2
        clip: true
        radius: 5

        NumberAnimation on heatingValue{
            id: heatingAnimation
            running: false
            from: 0; to: 100
            duration: waitingTime
            onStopped: {
                progress.value = 0
                progress.heatingValue = 0
                progressText.text = "Hold button to Heating..."
                stopHeated();
            }
        }

        NumberAnimation on value {
            id: pressAnimation
            running: false
            from: 0; to: 100
            duration: 500

            onStopped: {
                if (progress.value === 100){
                    if (heatingAnimation.running){
                        heatingAnimation.pause();
                        heatingAnimation.from = Math.max(progress.heatingValue, 100 * (waitingTime-rearEdgeTime) /waitingTime);
                        heatingAnimation.duration = (1 - heatingAnimation.from / 100) * waitingTime;
                        heatingAnimation.restart();
                        progressText.text = "Wait for stoping..."

                        emergencyStop();
                    }
                    else
                    {
                        progressText.text = "Heating. Hold to stop..."
                        heatingAnimation.from = 0;
                        heatingAnimation.duration = waitingTime;
                        heatingAnimation.start();

                        startHeating();
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
