#ifndef LEADERBOARDHELPER_H_
#define LEADERBOARDHELPER_H_

#include <QObject>
#include <QtNetwork/qnetworkreply.h>

class QNetworkAccessManager;
class QNetworkRequest;

// This class implements the Parse REST API
class LeaderboardHelper: public QObject
{
    Q_OBJECT
    enum State {
        Idle = 0,
        SignUpInProgress,
        SignInInProgress,
        CheckUserExists
    };
public:

    Q_ENUMS(Errors)

    enum Errors {
        GeneralError = -1,
        GeneralNoError = 0,
        SignUpAccountExists = 1,
        SignUpNetworkError = 2,
        SignInWrongCredentials = 3,
        SignInUserDoNotExist = 4,
        UserExists = 5,
        UserDoNotExists = 6
    };

    /**
     * This property will be set to true if we already signed in
     */
    Q_PROPERTY(bool signedIn READ signedIn NOTIFY signedInChanged )

    LeaderboardHelper(QObject *parent = NULL);
    virtual ~LeaderboardHelper();

public slots:
    void initialize();
    void signUp(const QString &login, const QString &password);
    bool signedIn();
    void signIn(const QString &login, const QString &password);
    void signOut();
    void checkUserExists(const QString &login);

signals:
    void signedInChanged(bool);
    void signUpCompleted(bool success, int error);
    void signInCompleted(bool success, int error);
    void userExistsCompleted(bool success, int result);

private slots:
    void onReplyFinished();
    void onReplyError(QNetworkReply::NetworkError networkError);

private:
    void configureStandardRequest(QNetworkRequest &request, const QString &path);
    void connectReplySignals();
    void handleSignUpResult();
    void handleSignUpResultError(QNetworkReply::NetworkError error);
    void handleSignInResult();
    void handleSignInResultError(QNetworkReply::NetworkError error);
    void handleUserExistsResult();
    void handleUserExistsResultError(QNetworkReply::NetworkError error);

private:
    QNetworkAccessManager *mNetworkManager;
    QNetworkReply *mReply;
    State mState;
    QString mAppId;
    QString mAPIKey;
    QString mTmpLogin;
};

#endif /* LEADERBOARDHELPER_H_ */
