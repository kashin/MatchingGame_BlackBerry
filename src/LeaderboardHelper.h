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
        SignInInProgress
    };
public:

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

signals:
    void signedInChanged(bool);
    void signUpCompleted(bool success);
    void signInCompleted(bool success);

private slots:
    void onReplyFinished();
    void onReplyError(QNetworkReply::NetworkError networkError);


private:
    void configureStandardRequest(QNetworkRequest &request, const QString &path);
    void connectReplySignals();
    void handleSignUpResult();

private:
    QNetworkAccessManager *mNetworkManager;
    QNetworkReply *mReply;
    State mState;
    QString mAppId;
    QString mAPIKey;
};

#endif /* LEADERBOARDHELPER_H_ */
