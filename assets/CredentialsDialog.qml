import bb.cascades 1.4
import don.matching 1.0

Dialog {
    id: credentialsDialog
    property bool isSignInDialog: true
    property bool userDoNotExistsChecked: true
    Container {
        id: mainContainer
        layout: DockLayout {}
        background: Color.create(0.0, 0.0, 0.0, 0.5)
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        Container {
            id: contentContainer
            background: Color.Black
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Center
            Label {
                id: credentialsDialogLabel
                property string baseDescriptionText: qsTr("Enter your loging and password to ")
                property string signInText: qsTr("Sign In")
                property string signUpText: qsTr("Sign Up")
                margin.topOffset: 10
                margin.bottomOffset: 10
                text: baseDescriptionText + (credentialsDialog.isSignInDialog ? signInText : signUpText)
                textStyle.color: Color.White
            }
            Label {
                id: errorLabel
                property string generalErrorText: qsTr("Something went wrong.\nCheck your credenials and internet connectivity and try again.")
                property string userExistsText: qsTr("Sorry, this account name already exists, try to peak another one.")
                margin.topOffset: 10
                margin.bottomOffset: 10
                text: generalErrorText
                multiline: true
                autoSize.maxLineCount: 2
                textStyle.color: Color.Red
                visible: false
            }
            Label {
                id: loginLabel
                margin.topOffset: 10
                margin.bottomOffset: 10
                textStyle.color: Color.White
                text: qsTr("Login")
            }
            TextField {
                id: loginField
                margin.topOffset: 10
                margin.bottomOffset: 10
                hintText: qsTr("Enter Login")
                inputMode: TextFieldInputMode.Text
                onFocusedChanged: {
                    if (!focused && signUpCheckBox.checked) {
                        errorLabel.visible = false;
                        leaderboardHelper.checkUserExists(text);
                    }
                    userDoNotExistsChecked = true;
                    credentialsDialog.updateSubmitButtonState();
                }
                onTextChanging: {
                    updateSubmitButtonState();
                }
            }
            Label {
                id: passwordLabel
                margin.topOffset: 10
                margin.bottomOffset: 10
                textStyle.color: Color.White
                text: qsTr("Password")
            }
            TextField {
                id: passwordField
                hintText: qsTr("Enter Password")
                inputMode: TextFieldInputMode.Password
                margin.topOffset: 10
                margin.bottomOffset: 10
                onTextChanging: {
                    updateSubmitButtonState();
                }
                onFocusedChanged: {
                    if (!focused) {
                        updateSubmitButtonState();
                    }
                }
            }
            CheckBox {
                id: signUpCheckBox
                text: qsTr("create new account?")
                onCheckedChanged: {
                    credentialsDialog.userDoNotExistsChecked = true;
                    credentialsDialog.isSignInDialog = !checked;
                }
                margin.topOffset: 10
                margin.bottomOffset: 10
            }
            Container {
                margin.topOffset: 10
                margin.bottomOffset: 10
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Button {
                    id: cancelButton
                    text: qsTr("Cancel")
                    onClicked: {
                        credentialsDialog.close();
                    }
                }
                Button {
                    id: submitButton
                    property string signInBtnText: qsTr("Sign In")
                    property string signUpBtnText: qsTr("Sign Up")
                    text: signInBtnText
                    enabled: false
                    onClicked: {
                        if (credentialsDialog.isSignInDialog) {
                            leaderboardHelper.signIn(loginField.text, passwordField.text);
                        } else {
                            leaderboardHelper.checkUserExists(loginField.text);
                        }
                        submitButton.enabled = false;
                    }
                }
            } // Internal container
        }
    } // mainContainer

    function updateSubmitButtonState() {
        if (userDoNotExistsChecked) {
            if (passwordField.text.length > 0 && loginField.text.length > 0) {
                submitButton.enabled = true;
            } else {
                submitButton.enabled = false;
            }
        } else {
            submitButton.enabled = false;
        }
        if (credentialsDialog.isSignInDialog) {
            submitButton.text = submitButton.signInBtnText;
        } else {
            submitButton.text = submitButton.signUpBtnText;
        }
    }

    function signInUpdated() {
        updateSubmitButtonState();
        if (credentialsDialog.isSignInDialog) {
            credentialsDialogLabel.text = credentialsDialogLabel.baseDescriptionText + credentialsDialogLabel.signInText;
        } else {
            credentialsDialogLabel.text = credentialsDialogLabel.baseDescriptionText + credentialsDialogLabel.signUpText;
        }
    }

    function showGeneralErrorLabel() {
        errorLabel.text = errorLabel.generalErrorText;
        errorLabel.visible = true;
    }

    onIsSignInDialogChanged: {
        console.log("onIsSignInDialogChanged");
        signInUpdated();
        errorLabel.visible = false;
    }

    onUserDoNotExistsCheckedChanged: {
        updateSubmitButtonState();
    }

    onCreationCompleted: {
        leaderboardHelper.initialize();
    }

    onClosed: {
        errorLabel.visible = false;
        submitButton.enabled = true;
    }

    attachedObjects: [
        Timer {
            id: signUpTimer
            interval: 0
            singleShot: true
            onTimeout: {
                console.log("timeout expired");
                leaderboardHelper.signUp(loginField.text, passwordField.text);
            }
        },
        LeaderboardHelper {
            id: leaderboardHelper
            onSignedInChanged: {
                credentialsDialog.signInUpdated();
            }
            onSignInCompleted: {
                console.log("sign in completed: " + success + ", error:" + error);
                if (success) {
                    switch(error) {
                        case LeaderboardHelper.GeneralNoError:
                            credentialsDialog.close();
                            break;
                        case LeaderboardHelper.GeneralError:
                            credentialsDialog.showGeneralErrorLabel();
                            break;
                        default:
                            console.log("unhandled error");
                            break;
                    }
                } else {
                    credentialsDialog.showGeneralErrorLabel();
                }
            }
            onSignUpCompleted: {
                console.log("sign up completed: " + success + ", error:" + error);
                if (success) {
                    switch(error) {
                        case LeaderboardHelper.GeneralNoError:
                            credentialsDialog.close();
                            break;
                        case LeaderboardHelper.SignUpAccountExists:
                            errorLabel.text = errorLabel.userExistsText;
                            errorLabel.visible = true;
                            break;
                        default:
                            console.log("unhandled error");
                            break;
                    }
                } else {
                    credentialsDialog.showGeneralErrorLabel();
                }
            }
            onUserExistsCompleted: {
                console.log("user exists completed with result:" + result);
                if (result === LeaderboardHelper.UserExists) {
                    errorLabel.text = errorLabel.userExistsText;
                    errorLabel.visible = true;
                    credentialsDialog.userDoNotExistsChecked = false;
                } else if (result === LeaderboardHelper.UserDoNotExists) {
                    credentialsDialog.userDoNotExistsChecked = true;
                    if (!submitButton.enabled) {
                        signUpTimer.start();
                    }
                } else {
                    credentialsDialog.showGeneralErrorLabel();
                }
                updateSubmitButtonState();
            }
        }
    ]
}