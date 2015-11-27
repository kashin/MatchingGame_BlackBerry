import bb.cascades 1.4
import bb.system 1.0
import don.matching 1.0

Page {
    id: page
    Container {
        Label {
            id: descriptionLabel
            text: qsTr("Find the extra image on the left side of the screen. You have 60 seconds to do it")
            rightMargin: 10
            leftMargin: 10
            topMargin: 10
            bottomMargin: 10
            autoSize.maxLineCount: 2
            multiline: true
            layoutProperties: StackLayoutProperties {
                spaceQuota: -1
            }
        }
        Container {
            topMargin: 5
            bottomMargin: 5
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Label {
                id: currentImgLabel
                property string baseTextValue: qsTr("Shown images: ")
                text: baseTextValue + "0"
                margin.topOffset: 10
                margin.bottomOffset: 10
                margin.leftOffset: 10
                margin.rightOffset: 10
                layoutProperties: StackLayoutProperties {
                    spaceQuota: -1
                }
            }
            Label {
                id: currentLevelLabel
                property string baseTextValue: qsTr("Current Level: ")
                text: baseTextValue
                margin.topOffset: 10
                margin.bottomOffset: 10
                margin.leftOffset: 10
                margin.rightOffset: 10
                layoutProperties: StackLayoutProperties {
                    spaceQuota: -1
                }
            }
            layoutProperties: StackLayoutProperties {
                spaceQuota: -1
            }
        }
        Button {
            id: startButton
            text: qsTr("Start")
            topMargin: 10
            bottomMargin: 10
            horizontalAlignment: HorizontalAlignment.Center
            appearance: ControlAppearance.Primary
            onClicked: {
                mainScreenWebView.postMessage("startGame");
                descriptionLabel.visible = false;
                currentLevelLabel.text = currentLevelLabel.baseTextValue + '1';
                currentImgLabel.text = currentImgLabel.baseTextValue + appSettings.difficulty;
            }
        }
        Label {
            id: timerLabel
            text: '0'
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            topMargin: 10
            bottomMargin: 10
        }
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            WebView {
                id: mainScreenWebView
                property int scoreValue: 0
                property int levelValue: 1
                url: "local:///assets/html/face_matching.html"
                onMessageReceived: {
                    console.log("received message: " + message.data);
                    if (message.data.indexOf("imgCounter") >= 0) {
                        var data = message.data.substring(message.data.indexOf(":") + 1);
                        currentImgLabel.text = currentImgLabel.baseTextValue + data;
                        scoreValue = data - appSettings.difficulty;
                        roundTimer.restartRoundTimer();
                    } else if (message.data.indexOf("currentLevel") >= 0) {
                        var data = message.data.substring(message.data.indexOf(":") + 1);
                        currentLevelLabel.text = currentLevelLabel.baseTextValue + data;
                        levelValue = data - 1;
                    } else if (message.data.indexOf("gameOver") >= 0) {
                        var data = message.data.substring(message.data.indexOf(":") + 1);
                        console.log("game over: " + data);
                        page.showGameOverDialog()
                    } else if (message.data.indexOf("readyToStart") >= 0) {
                        console.log("game is ready to start");
                    } else if (message.data === "gameLoaded") {
                        console.log("posting difficulty message: " + appSettings.difficulty);
                        postMessage("difficulty:" + appSettings.difficulty);
                    } else if (message.data === 'gameStarted') {
                        console.log('game is started');
                        roundTimer.start();
                    } else {
                        console.log('received unhandled message:' + message.data);
                    }
                }
            }
            attachedObjects: LayoutUpdateHandler {
                onLayoutFrameChanged: {
                    mainScreenWebView.preferredWidth = layoutFrame.width;
                    mainScreenWebView.preferredHeight = layoutFrame.height;
                }
            }
        }
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            title: qsTr("Back to Main Menu")
            onTriggered: {
                navigationPane.pop();
            }
        }
    }

    function showGameOverDialog() {
        roundTimer.stopTimer();
        gameOverDialog.body = gameOverDialog.bodyBaseText.arg(mainScreenWebView.scoreValue);
        gameOverDialog.show();
    }

    attachedObjects: [
        Settings {
            id: appSettings
        },
        Timer {
            id: roundTimer
            property int counter: 0
            property int maxCounterValue: 60 // for 60 seconds
            interval: 1000 // 1 second
            singleShot: true

            function stopTimer() {
                counter = 0;
                stop();
            }

            function restartRoundTimer() {
                stop();
                counter = 0;
                start();
            }

            onTimeout: {
                ++counter;
                if (counter >= maxCounterValue) {
                    stopTimer();
                    mainScreenWebView.postMessage("gameOverTimer");
                    page.showGameOverDialog();
                } else {
                    start();
                }
            }
            onCounterChanged: {
                timerLabel.text = '' + counter;
            }
        },
        SystemDialog {
            id: gameOverDialog
            property string bodyBaseText: qsTr("Your score is %1. Do you want to submit your score?")
            buttonAreaLimit: 2
            title: qsTr("Game Over")
            onFinished: {
                descriptionLabel.visible = true;
                if (gameOverDialog.result ===
                    SystemUiResult.CancelButtonSelection) {
                    console.log('do not submit the result');
                } else {
                    leaderboardHelper.submitNewScore(mainScreenWebView.scoreValue,
                        appSettings.difficulty, mainScreenWebView.levelValue);
                    successfullSubmission.show();
                }
            }
        },
        LeaderboardHelper {
            id: leaderboardHelper
            onSubmitNewScoreCompleted: {
                if (success) {
                    successfullSubmission.progress = 100;
                } else {
                    successfullSubmission.cancel();
                }
            }
        },
        SystemProgressToast {
            id: successfullSubmission
            body: qsTr("Submitting results.")
            progress: 0
            statusMessage: qsTr("In progress...")
            state: SystemUiProgressState.Active
            position: SystemUiPosition.MiddleCenter
        }
    ]
}
