import bb.cascades 1.4
import don.matching 1.0

Dialog {
    id: credentialsDialog
    property bool isSignInDialog: false
    Container {
        id: mainContainer
        layout: DockLayout {
        }
        background: Color.create(0.0, 0.0, 0.0, 0.5)
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        Container {
            id: contentContainer
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Center
            Label {
                id: credentialsDialogLabel
                property string baseDescriptionText: qsTr("Enter your loging and password to ")
                property string signInText: qsTr("Sign In")
                property string signUpText: qsTr("Sign Up")
                text: baseDescriptionText + (credentialsDialog.isSignInDialog ? signInText : signUpText)
            }
            Label {
                id: errorLabel
                text: qsTr("Can not sign in to your account.\n Check your credenials and internet connectivity and try again.")
                multiline: true
                autoSize.maxLineCount: 2
                textStyle.color: Color.Red
                visible: false
            }
            Label {
                id: loginLabel
                text: qsTr("Login")
            }
            TextField {
                id: loginField
                hintText: qsTr("Enter Login")
                inputMode: TextFieldInputMode.Text
            }
            Label {
                id: passwordLabel
                text: qsTr("Password")
            }
            TextField {
                id: passwordField
                hintText: qsTr("Enter Password")
                inputMode: TextFieldInputMode.Password
            }
            Container {
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
                    text: signUpBtnText
                    onClicked: {
                        if (credentialsDialog.isSignInDialog) {
                            leaderboardHelper.signIn(loginField.text, passwordField.text);
                        } else {
                            leaderboardHelper.signUp(loginField.text, passwordField.text);
                        }
                        submitButton.enabled = false;
                    }
                }
            } // Internal container
        }
    } // mainContainer
    onIsSignInDialogChanged: {
        signInUpdated();
    }

    function signInUpdated() {
        submitButton.enabled = true;
        if (isSignInDialog) {
            submitButton.text = submitButton.signInBtnText;
            credentialsDialogLabel.text = credentialsDialogLabel.baseDescriptionText + credentialsDialogLabel.signInText;
        } else {
            submitButton.text = submitButton.signUpBtnText;
            credentialsDialogLabel.text = credentialsDialogLabel.baseDescriptionText + credentialsDialogLabel.signUpText;
        }
    }

    function handleSignOperation(success) {
        if (success) {
            credentialsDialog.close();
        } else {
            errorLabel.visible = true;
            submitButton.enabled = true;
        }
    }

    onCreationCompleted: {
        leaderboardHelper.initialize();
    }
    attachedObjects: [
        LeaderboardHelper {
            id: leaderboardHelper
            onSignedInChanged: {
                credentialsDialog.signInUpdated();
            }
            onSignInCompleted: {
                console.log("sign in completed");
                credentialsDialog.close();
            }
            onSignUpCompleted: {
                console.log("sign up completed");
                credentialsDialog.close();
            }
            
        }
    ]
}