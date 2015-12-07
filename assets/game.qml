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
            textStyle {
                base: SystemDefaults.TextStyles.BodyText
            }
            autoSize.maxLineCount: 3
            multiline: true
            horizontalAlignment: HorizontalAlignment.Center
            layoutProperties: StackLayoutProperties {
                spaceQuota: -1
            }
        }
        Divider {
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
            property int buttonState: 0 // 0 - Start, 1 - Pause, 2 - Resume
            property string startBtnText: qsTr("Start")
            property string pauseBtnText: qsTr("Pause")
            property string resumeBtnText: qsTr("Resume")
            text: startBtnText
            topMargin: 10
            bottomMargin: 10
            horizontalAlignment: HorizontalAlignment.Center
            appearance: ControlAppearance.Primary

            function pauseGame() {
                if (startButton.buttonState === 1) {
                    mainScreenWebView.pauseGame();
                    text = resumeBtnText;
                    buttonState = 2;
                }
            }

            function resumeGame() {
                if (startButton.buttonState === 2) {
                    mainScreenWebView.resumeGame();
                    text = pauseBtnText;
                    buttonState = 1;
                }
            }

            function gameOver() {
                if (startButton.buttonState > 0) {
                    Application.menuEnabled = true;
                    startButton.buttonState = 0;
                    startButton.text = startButton.startBtnText;
                }
            }

            onClicked: {
                switch (startButton.buttonState) {
                    case 0:
                        {
                            Application.menuEnabled = false;
                            mainScreenWebView.startNewGame();
                            descriptionLabel.visible = false;
                            currentLevelLabel.text = currentLevelLabel.baseTextValue + '1';
                            currentImgLabel.text = currentImgLabel.baseTextValue + appSettings.difficulty;
                            text = pauseBtnText;
                            buttonState = 1;
                            break;
                        }
                    case 1:
                        {
                            Application.menuEnabled = true;
                            pauseGame();
                            break;
                        }
                    case 2:
                        {
                            Application.menuEnabled = false;
                            resumeGame();
                            break;
                        }
                    default:
                        {
                            console.log("unhandled button state");
                            break;
                        }
                }
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

                function startNewGame() {
                    mainScreenWebView.scoreValue = 0;
                    mainScreenWebView.levelValue = 0;
                    mainScreenWebView.postMessage("startGame");
                }

                function pauseGame() {
                    mainScreenWebView.postMessage("pauseGame");
                    roundTimer.pauseTimer();
                }

                function resumeGame() {
                    mainScreenWebView.postMessage("resumeGame");
                    mainScreenWebView.visible = true;
                    roundTimer.resumeTimer();
                }

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
                        page.showGameOverDialog();
                        startButton.gameOver();
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

            function resumeTimer() {
                start();
            }

            function pauseTimer() {
                stop();
            }

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
                } else if (appSettings.playerName.length == 0) {
                    enterPlayerNamePromt.show();
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
        SystemPrompt {
            id: enterPlayerNamePromt
            body: qsTr("Please, enter your player name.")
            inputField.emptyText: appSettings.defaulPlayerName()
            onFinished: {
                if (gameOverDialog.result ===
                SystemUiResult.ConfirmButtonSelection) {
                    appSettings.playerName = inputFieldTextEntry();
                    leaderboardHelper.submitNewScore(mainScreenWebView.scoreValue,
                    appSettings.difficulty, mainScreenWebView.levelValue);
                    successfullSubmission.show();
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
