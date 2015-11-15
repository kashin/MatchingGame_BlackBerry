#include "LeaderboardHelper.h"

// Qt
#include <QStringList>
#include <QFile>
#include <QDebug>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>


LeaderboardHelper::LeaderboardHelper(QObject *parent)
    : QObject(parent),
      mNetworkManager(new QNetworkAccessManager(this))
{
}

LeaderboardHelper::~LeaderboardHelper()
{
}

void LeaderboardHelper::initialize()
{
    QFile file("app/native/assets/parseKey/parse_key.data");
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "can't open file:" << file.errorString();
        return;
    }
    QByteArray appIdValue = file.readLine(200);
    QByteArray appKeyValue = file.readLine(200);
    mAppId = QString::fromUtf8(appIdValue.data(), appIdValue.length()).split(':').last().simplified();
    mAPIKey = QString::fromUtf8(appKeyValue.data(), appKeyValue.length()).split(':').last();
    qDebug() << "mAppId=" << mAppId << "mAPIKey=" << mAPIKey;
}

void LeaderboardHelper::signUp(const QString& login, const QString& password)
{
    Q_UNUSED(login);
    Q_UNUSED(password);
    // TODO: implement
}

bool LeaderboardHelper::signedIn()
{
    // TODO: implement
    return true;
}
