import bb.cascades 1.4

NavigationPane {
    id: navigationPane
    Page {
        Container {
            layout: DockLayout {}
            Container {
                Button {
                    id: startGameButton
                    text: qsTr("Start Game")
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Top
                    appearance: ControlAppearance.Primary
                    onClicked: {
                        startGameAction.triggered();
                    }
                }
                Button {
                    id: openLeaderboardButton
                    text: qsTr("Leaderboard")
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Bottom
                    appearance: ControlAppearance.Primary
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
        }
        actions: ActionItem {
            id: startGameAction
            title: qsTr("Start Game")
            ActionBar.placement: ActionBarPlacement.OnBar
            shortcuts: [
                Shortcut {
                    key: "s"
                    onTriggered: {
                        startGameAction.triggered();
                    }
                }
            ]
            onTriggered: {
                var gamePage = gamePageDefinition.createObject();
                navigationPane.push(gamePage);
            }
            attachedObjects: [
                ComponentDefinition {
                    id: gamePageDefinition
                    source: "game.qml"
                }
            ]
        }
    }
}
