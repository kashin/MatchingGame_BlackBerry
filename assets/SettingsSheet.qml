import bb.cascades 1.4
import don.matching 1.0

Sheet {
    id: settingsSheet
    content: Page {
        titleBar: TitleBar {
            appearance: TitleBarAppearance.Plain
            acceptAction: ActionItem {
                title: qsTr("Close")
                onTriggered: {
                    settingsSheet.close();
                }
            }
        }
        Container {
            Label {
                id: difficultyLabel
                text: qsTr("Difficulty Level")
            }
            DropDown {
                id: difficultyDropDown
                title: qsTr("Difficulty")
                enabled: true
                onSelectedIndexChanged: {
                    console.log("SelectedIndex: " + selectedIndex);
                }
                Option {
                    text: qsTr("Easy")
                    value: Settings.EasyDifficulty
                    selected: false
                    onSelectedChanged: {
                        if (selected == true) {
                            console.log("Setting set to 3.");
                            appSettings.difficulty = Settings.EasyDifficulty;
                        }
                    }
                }
                
                Option {
                    text: qsTr("Normal")
                    value: Settings.NormalDifficulty
                    onSelectedChanged: {
                        if (selected == true) {
                            console.log("Setting set to 5.");
                            appSettings.difficulty = Settings.NormalDifficulty;
                        }
                    }
                }
                
                Option {
                    text: qsTr("Hard")
                    value: Settings.HardDifficulty
                    onSelectedChanged: {
                        if (selected == true) {
                            console.log("Setting set to 7.");
                            appSettings.difficulty = Settings.HardDifficulty;
                        }
                    }
                }
            } // Difficulty DropDown
            Container {
                id: userCredentials
                visible: false
                Label {
                    id: loginLabel
                    text: qsTr("Login")
                }
                TextField {
                    id: loginField
                    hintText: qsTr("Enter Login")
                }
                Label {
                    id: passwordLabel
                    text: qsTr("Login")
                }
                TextField {
                    id: passwordField
                    hintText: qsTr("Enter Password")
                }
            }
        }
    } // Page
    attachedObjects: [
        Settings {
            id: appSettings
        }
    ]
    onCreationCompleted: {
        switch(appSettings.difficulty) {
            case Settings.EasyDifficulty:
                difficultyDropDown.selectedIndex = 0;
                break;
            case Settings.NormalDifficulty:
                difficultyDropDown.selectedIndex = 1;
                break;
            case Settings.HardDifficulty:
                difficultyDropDown.selectedIndex = 2;
                break;
            default:
                console.log("Unknown difficulty level");
                break;
        }
    }
}