import bb.cascades 1.4

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
        Label {
            id: currentImgCounter
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
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            WebView {
                id: mainScreenWebView
                url: "local:///assets/html/face_matching.html"
                onMessageReceived: {
                    if (message.data.indexOf("imgCounter") >= 0) {
                        var data = message.data.substring(message.data.indexOf(":") + 1);
                        currentImgCounter.text = currentImgCounter.baseTextValue + data;
                    } else if (message.data.indexOf("gameOver") >= 0) {
                        var data = message.data.substring(message.data.indexOf(":") + 1);
                        console.log("received message data: " + data);
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
}
