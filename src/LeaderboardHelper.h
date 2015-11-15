#ifndef LEADERBOARDHELPER_H_
#define LEADERBOARDHELPER_H_

#include <QObject>
#include <QtNetwork/qnetworkreply.h>

class QNetworkAccessManager;

// This class implements the Parse REST API
class LeaderboardHelper: public QObject
{
    Q_OBJECT
public:

    /**
     * This property will be set to true if we already signed in
     */
    Q_PROPERTY(bool signedIn READ  signedIn)

    LeaderboardHelper(QObject *parent = 0);
    virtual ~LeaderboardHelper();

public slots:
    void initialize();
    void signUp(const QString &login, const QString &password);
    bool signedIn();

private:
    QNetworkAccessManager *mNetworkManager;
    QString mAppId;
    QString mAPIKey;
};

#endif /* LEADERBOARDHELPER_H_ */
