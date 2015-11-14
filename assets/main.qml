/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.4

Page {
    Container {
        WebView {
            id: mainScreenWebView
            url: "local:///assets/html/face_matching.html"
            onMessageReceived: {
                if (message.data.indexOf("imgCounter") >= 0) {
                    var data = message.data.substring(message.data.indexOf(":") + 1);
                    console.log("received img message data: " + data);
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
