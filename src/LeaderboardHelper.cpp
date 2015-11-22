#include "LeaderboardHelper.h"
#include "SimpleJSONCreator.h"
#include "JSONObject.h"
#include "Settings.h"

// Qt
#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QUrl>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>

namespace {
    // Request data
    static const QByteArray APP_ID_HEADER("X-Parse-Application-Id");
    static const QByteArray REST_API_HEADER("X-Parse-REST-API-Key");
    static const QByteArray CLIENT_API_HEADER("X-Parse-Client-Key");
    static const QByteArray REVOCABLE_SESSION_HEADER("X-Parse-Revocable-Session");
    static const QLatin1String API_ADDRESS("api.parse.com");
    static const QLatin1String SCHEME("https");
    static const QLatin1String USERS_PATH("/1/users");
    static const QLatin1String LOGIN_PATH("/1/login");
    static const QLatin1String CONTENT_TYPE_JSON_HEADER_VALUE("application/json");

    // JSON data
    static const QLatin1String USERSNAME_FIELD("username");
    static const QLatin1String PASSWORD_FIELD("password");
    static const QLatin1String CRATED_AT_FIELD("createdAt");
    static const QLatin1String OBJECT_ID_FIELD("objectId");
    static const QLatin1String SESSION_TOKEN_FIELD("sessionToken");
    static const QLatin1String RESULTS_FIELD("results");
}

LeaderboardHelper::LeaderboardHelper(QObject *parent)
    : QObject(parent),
      mNetworkManager(new QNetworkAccessManager(this)),
      mReply(NULL),
      mState(Idle)
{
}

LeaderboardHelper::~LeaderboardHelper()
{
    if (mReply) {
        mReply->deleteLater();
        mReply = NULL;
    }
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

void LeaderboardHelper::signUp(const QString &login, const QString &password)
{
    qDebug() << "sending signUp";
    QNetworkRequest request;
    configureStandardRequest(request, USERS_PATH);
    request.setHeader(QNetworkRequest::ContentTypeHeader, CONTENT_TYPE_JSON_HEADER_VALUE);
    SimpleJSONCreator *jsonCreator = new SimpleJSONCreator(this);
    jsonCreator->addValue(USERSNAME_FIELD, login);
    jsonCreator->addValue(PASSWORD_FIELD, password);
    mState = SignUpInProgress;
    mReply = mNetworkManager->post(request, jsonCreator->getJsonData());
    connectReplySignals();
    jsonCreator->deleteLater();
}

bool LeaderboardHelper::signedIn()
{
    Settings settings;
    return !settings.sessionToken().isEmpty();
}

void LeaderboardHelper::configureStandardRequest(QNetworkRequest &request, const QString &path)
{
    QUrl url;
    url.setHost(API_ADDRESS);
    url.setScheme(SCHEME);
    url.setPath(path);
    request.setUrl(url);
    request.setRawHeader(APP_ID_HEADER, mAppId.toUtf8());
    request.setRawHeader(CLIENT_API_HEADER, mAPIKey.toUtf8());
    request.setRawHeader(REVOCABLE_SESSION_HEADER, "1");
}

void LeaderboardHelper::connectReplySignals()
{
    Q_ASSERT(mReply);
    connect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onReplyError(QNetworkReply::NetworkError)));
}

void LeaderboardHelper::onReplyFinished()
{
    qDebug() << "replyFinished. State=" << mState;
    if (!mReply) {
        return;
    }
    switch(mState) {
        case Idle:
            Q_ASSERT(!"received reply in Idle state");
            break;
        case SignUpInProgress:
            handleSignUpResult();
            break;
        case CheckUserExists:
            handleUserExistsResult();
            break;
        case SignInInProgress:
            handleSignInResult();
            break;
        default:
            qCritical() << "unhandled leaderboard state" << mState;
            break;
    }
    mState = Idle;
    mReply->deleteLater();
    mReply = NULL;
}

void LeaderboardHelper::onReplyError(QNetworkReply::NetworkError networkError)
{
    qDebug() << "reply error. State=" << mState << "error:" << networkError;
    qCritical() << "network request failed with error" << networkError;
    qCritical() << "replyHeaders: " << mReply->rawHeaderPairs();
    switch(mState) {
        case Idle:
            Q_ASSERT(!"received reply in Idle state");
            break;
        case SignUpInProgress:
            handleSignUpResultError(networkError);
            break;
        case CheckUserExists:
            handleUserExistsResultError(networkError);
            break;
        case SignInInProgress:
            handleSignInResultError(networkError);
            break;
        default:
            qCritical() << "unhandled leaderboard state" << mState;
            break;
    }
}

void LeaderboardHelper::signIn(const QString &login, const QString &password)
{
    qDebug() << "sending signIn";
    QNetworkRequest request;
    configureStandardRequest(request, LOGIN_PATH);
    mState = SignInInProgress;
    QUrl url = request.url();
    url.addQueryItem(USERSNAME_FIELD.latin1(), QUrl::toPercentEncoding(login.toUtf8()));
    url.addQueryItem(PASSWORD_FIELD.latin1(), QUrl::toPercentEncoding(password.toUtf8()));
    request.setUrl(url);
    mReply = mNetworkManager->get(request);
    connectReplySignals();
}

void LeaderboardHelper::signOut()
{
    Settings settings;
    settings.setSessionToken(QString());
}

void LeaderboardHelper::handleSignUpResult()
{
    QNetworkReply::NetworkError error = mReply->error();
    const QList<QPair<QByteArray, QByteArray> > &headers = mReply->rawHeaderPairs();
    const QByteArray &data = mReply->readAll();
    qDebug() << "all reply headers:" << headers;
    qDebug() << "received data:" << data;
    bool success = error == QNetworkReply::NoError;
    if (success) {
        JSONObject *parsedObject = new JSONObject(this);
        parsedObject->parseJSONData(data);
        typedef QPair<QString, QString> JsonValue;
        foreach(const JsonValue &value, parsedObject->values()) {
            if (value.first.compare(CRATED_AT_FIELD) == 0) {
                qDebug() << "user created at:" << value.second;
            } else if (value.first.compare(OBJECT_ID_FIELD) == 0) {
                qDebug() << "objectId:" << value.second;
            } else if (value.first.compare(SESSION_TOKEN_FIELD) == 0) {
                qDebug() << "sessionToken:" << value.second;
                Settings settings;
                settings.setSessionToken(value.second);
            }
        }
        parsedObject->deleteLater();
    } else {
        qWarning() << "request failed with error:" << error;
    }
    emit signUpCompleted(success, GeneralNoError);
}

void LeaderboardHelper::handleSignUpResultError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
}

void LeaderboardHelper::checkUserExists(const QString& login)
{
    qDebug() << "sending user exists";
    mTmpLogin = login;
    QNetworkRequest request;
    configureStandardRequest(request, USERS_PATH);
    mState = CheckUserExists;
    mReply = mNetworkManager->get(request);
    connectReplySignals();
}

void LeaderboardHelper::handleSignInResult()
{
    QNetworkReply::NetworkError error = mReply->error();
    const QList<QPair<QByteArray, QByteArray> > &headers = mReply->rawHeaderPairs();
    const QByteArray &data = mReply->readAll();
    qDebug() << "all reply headers:" << headers;
    qDebug() << "received data:" << data;
    bool success = error == QNetworkReply::NoError;
    int operationResult = GeneralError;
    if (success) {
        JSONObject *parsedObject = new JSONObject(this);
        parsedObject->parseJSONData(data);
        typedef QPair<QString, QString> JsonValue;
        foreach(const JsonValue &value, parsedObject->values()) {
            if (value.first.compare(CRATED_AT_FIELD) == 0) {
                qDebug() << "user created at:" << value.second;
            } else if (value.first.compare(OBJECT_ID_FIELD) == 0) {
                qDebug() << "objectId:" << value.second;
            } else if (value.first.compare(SESSION_TOKEN_FIELD) == 0) {
                qDebug() << "sessionToken:" << value.second;
                Settings settings;
                settings.setSessionToken(value.second);
                operationResult = GeneralNoError;
            }
        }
        parsedObject->deleteLater();
    } else {
        qWarning() << "request failed with error:" << error;
    }
    emit signInCompleted(success, operationResult);
}

void LeaderboardHelper::handleSignInResultError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
}

void LeaderboardHelper::handleUserExistsResult()
{
    QNetworkReply::NetworkError error = mReply->error();
    const QList<QPair<QByteArray, QByteArray> > &headers = mReply->rawHeaderPairs();
    const QByteArray &data = mReply->readAll();
    qDebug() << "all reply headers:" << headers;
    qDebug() << "received data:" << data;
    bool success = error == QNetworkReply::NoError;
    int result = GeneralNoError;
    if (success) {
        JSONObject *parsedObject = new JSONObject(this);
        parsedObject->parseJSONData(data);
        typedef QPair<QString, QList<JSONObject*> > JsonArray;
        typedef QPair<QString, QString> JsonValue;
        result = UserDoNotExists;
        foreach(const JsonArray &array, parsedObject->childArrayObjects()) {
            if (array.first.compare(RESULTS_FIELD) == 0) {
                foreach(JSONObject *object, array.second) {
                    foreach(const JsonValue &value, object->values()) {
                        if (value.first.compare(USERSNAME_FIELD) == 0) {
                            if (value.second.compare(mTmpLogin) == 0) {
                                result = UserExists;
                            }
                            break;
                        }
                    }
                    if (result == UserExists) {
                        break;
                    }
                }
                break;
            } else {
                qWarning() << "unexpected value:" << array.first;
            }
            if (result == UserExists) {
                break;
            }
        }
        parsedObject->deleteLater();
    } else {
        qWarning() << "request failed with error:" << error;
    }
    mTmpLogin.clear();
    emit userExistsCompleted(success, result);
}

void LeaderboardHelper::handleUserExistsResultError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
}
