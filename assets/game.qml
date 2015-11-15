import bb.cascades 1.4
import don.matching 1.0

Page {
    Container {
        Label {
            id: descriptionLabel
            text: qsTr("Find the extra image on the left side of the screen")
            margin.leftOffset: 10
            margin.rightOffset: 10
            layoutProperties: StackLayoutProperties {
                spaceQuota: -1
            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Label {
                id: currentImgLabel
                property string baseTextValue: qsTr("Shown images: ")
                text: baseTextValue + "5"
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
                text: baseTextValue + "1"
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
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            WebView {
                id: mainScreenWebView
                url: "local:///assets/html/face_matching.html"
                onMessageReceived: {
                    console.log("received message: " + message.data);
                    if (message.data.indexOf("imgCounter") >= 0) {
                        var data = message.data.substring(message.data.indexOf(":") + 1);
                        currentImgLabel.text = currentImgLabel.baseTextValue + data;
                    } else if (message.data.indexOf("currentLevel") >= 0) {
                        var data = message.data.substring(message.data.indexOf(":") + 1);
                        currentLevelLabel.text = currentLevelLabel.baseTextValue + data;
                    } else if (message.data.indexOf("gameOver") >= 0) {
                        var data = message.data.substring(message.data.indexOf(":") + 1);
                        console.log("game over: " + data);
                    } else if (message.data.indexOf("readyToStart") >= 0) {
                        postMessage("startGame");
                    } else if (message.data === "gameLoaded") {
                        console.log("posting difficulty message: " + appSettings.difficulty);
                        postMessage("difficulty:" + appSettings.difficulty);
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
    attachedObjects: [
        Settings {
            id: appSettings
        }
    ]
}
