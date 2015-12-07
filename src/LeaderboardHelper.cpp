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
    static const QByteArray SESSION_TOKEN_HEADER("X-Parse-Session-Token");
    static const QLatin1String API_ADDRESS("api.parse.com");
    static const QLatin1String SCHEME("https");
    static const QLatin1String USERS_PATH("/1/users");
    static const QLatin1String LOGIN_PATH("/1/login");
    static const QLatin1String GAME_SCORE_PATH("/1/classes/GameScore");
    static const QLatin1String APP_OPENED_PATH("/1/events/AppOpened");
    static const QLatin1String SETTINGS_OPENED_PATH("/1/events/SettingsOpened");
    static const QLatin1String CONTENT_TYPE_JSON_HEADER_VALUE("application/json");
    static const QLatin1String ORDER_QUERY_VALUE("order");
    static const QLatin1String LIMIT_QUERY_VALUE("limit");

    // JSON data
    static const QLatin1String USERSNAME_FIELD("username");
    static const QLatin1String PASSWORD_FIELD("password");
    static const QLatin1String CRATED_AT_FIELD("createdAt");
    static const QLatin1String OBJECT_ID_FIELD("objectId");
    static const QLatin1String SESSION_TOKEN_FIELD("sessionToken");
    static const QLatin1String RESULTS_FIELD("results");
    static const QLatin1String PLAYER_NAME_FIELD("playerName");
    static const QLatin1String DIFFICULTY_FIELD("difficulty");
    static const QLatin1String LEVEL_FIELD("level");
    static const QLatin1String SCORE_FIELD("score");
    static const QLatin1String SETTINGS_OPENED_FIELD("settingsVisited");
    static const QLatin1String DIMENSIONS_FIELD("dimensions");
}

LeaderboardHelper::LeaderboardHelper(QObject *parent)
    : QObject(parent),
      mNetworkManager(new QNetworkAccessManager(this)),
      mReply(NULL),
      mState(Idle)
{
    qRegisterMetaType<HighScore>("HighScore");
    initialize();
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
    if (!mAppId.isEmpty() && !mAPIKey.isEmpty()) {
        return;
    }
    QFile file("app/native/assets/parseKey/parse_key.data");
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "can't open file:" << file.errorString();
        return;
    }
    QByteArray appIdValue = file.readLine(200);
    QByteArray appKeyValue = file.readLine(200);
    mAppId = QString::fromUtf8(appIdValue.data(), appIdValue.length()).split(':').last().simplified();
    mAPIKey = QString::fromUtf8(appKeyValue.data(), appKeyValue.length()).split(':').last().simplified();
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

void LeaderboardHelper::submitNewScore(int newScore, int difficulty, int level)
{
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, CONTENT_TYPE_JSON_HEADER_VALUE);
    configureStandardRequest(request, GAME_SCORE_PATH);
    SimpleJSONCreator *jsonCreator = new SimpleJSONCreator(this);
    Settings appSettings;
    const QString &playerName = appSettings.playerName();
    jsonCreator->addValue(PLAYER_NAME_FIELD, playerName.isEmpty() ? appSettings.defaulPlayerName() : playerName);
    jsonCreator->addIntValue(SCORE_FIELD, newScore);
    jsonCreator->addValue(DIFFICULTY_FIELD, QString::number(difficulty));
    jsonCreator->addValue(LEVEL_FIELD, QString::number(level));
    mState = SubmitNewScoreInProgress;
    qDebug() << "submiting new score data:" << jsonCreator->getJsonData();
    qDebug() << "request:" << request.rawHeaderList();
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
    qDebug() << "settings headers with keys:" << mAppId << mAPIKey;
    request.setRawHeader(APP_ID_HEADER, mAppId.toUtf8());
    request.setRawHeader(REST_API_HEADER, mAPIKey.toUtf8());
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
        case SubmitNewScoreInProgress:
            handleSubmitNewScore();
            break;
        case QueryHighScoresInProgress:
            handleQueryHighScore();
            break;
        case SendingAnalyticsInProgress:
            // we are just ignoring anything related to analytics events
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
        case SubmitNewScoreInProgress:
            handleSubmitNewScoreError(networkError);
            break;
        case QueryHighScoresInProgress:
            handleQueryHighScoreError(networkError);
            break;
        case SendingAnalyticsInProgress:
            // we are just ignoring anything related to analytics events
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

void LeaderboardHelper::handleSubmitNewScore()
{
    QNetworkReply::NetworkError error = mReply->error();
    const QList<QPair<QByteArray, QByteArray> > &headers = mReply->rawHeaderPairs();
    const QByteArray &data = mReply->readAll();
    qDebug() << "all reply headers:" << headers;
    qDebug() << "received data:" << data;
    bool success = error == QNetworkReply::NoError;
    Errors operationError = GeneralNoError;
    if (success) {
        JSONObject *parsedObject = new JSONObject(this);
        parsedObject->parseJSONData(data);
        typedef QPair<QString, QString> JsonValue;
        foreach(const JsonValue &value, parsedObject->values()) {
            if (value.first.compare(CRATED_AT_FIELD) == 0) {
                qDebug() << "user created at:" << value.second;
            } else if (value.first.compare(OBJECT_ID_FIELD) == 0) {
                qDebug() << "objectId:" << value.second;
            } else {
                qCritical() << "un-expected value:" << value;
            }
        }
        parsedObject->deleteLater();
    } else {
        operationError = GeneralError;
        qWarning() << "request failed with error:" << error;
    }
    emit submitNewScoreCompleted(success, operationError);
}

void LeaderboardHelper::handleSubmitNewScoreError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
}

void LeaderboardHelper::queryHighScores(int limit)
{
    qDebug() << "sending query high scores";
    QNetworkRequest request;
    configureStandardRequest(request, GAME_SCORE_PATH);
    mState = QueryHighScoresInProgress;
    QUrl url = request.url();
    url.addQueryItem(ORDER_QUERY_VALUE.latin1(), QString('-').append(SCORE_FIELD).append(',').append(CRATED_AT_FIELD));
    url.addQueryItem(LIMIT_QUERY_VALUE.latin1(), QUrl::toPercentEncoding(QString::number(limit)));
    request.setUrl(url);
    mReply = mNetworkManager->get(request);
    connectReplySignals();
}

void LeaderboardHelper::handleQueryHighScore()
{
    QNetworkReply::NetworkError error = mReply->error();
    const QList<QPair<QByteArray, QByteArray> > &headers = mReply->rawHeaderPairs();
    const QByteArray &data = mReply->readAll();
    qDebug() << "all reply headers:" << headers;
    qDebug() << "received data:" << data;
    bool success = error == QNetworkReply::NoError;
    int operationResult = GeneralError;
    QList<HighScore> highScores;
    if (success) {
        JSONObject *parsedObject = new JSONObject(this);
        parsedObject->parseJSONData(data);
        typedef QPair<QString, QList<JSONObject*> > JsonArray;
        typedef QPair<QString, QString> JsonValue;
        typedef QPair<QString, int> JsonIntValue;
        operationResult = GeneralNoError;
        foreach(const JsonArray &array, parsedObject->childArrayObjects()) {
            if (array.first.compare(RESULTS_FIELD) == 0) {
                foreach(JSONObject *object, array.second) {
                    HighScore score;
                    foreach(const JsonValue &value, object->values()) {
                        if (value.first.compare(PLAYER_NAME_FIELD) == 0) {
                            score.playerName = value.second;
                        } else if (value.first.compare(LEVEL_FIELD) == 0) {
                            score.level = value.second.toUInt();
                        } else if (value.first.compare(DIFFICULTY_FIELD) == 0) {
                            score.difficulty = value.second.toUInt();
                        }
                    }
                    foreach (const JsonIntValue &value, object->intValues()) {
                        if (value.first.compare(SCORE_FIELD) == 0) {
                            score.score = value.second;
                            break;
                        }
                    }
                    qDebug() << "score:" << score.score << "; playerName:" << score.playerName;
                    highScores << score;
                }
                break;
            } else {
                qWarning() << "unexpected value:" << array.first;
            }
        }
        parsedObject->deleteLater();
    } else {
        qWarning() << "request failed with error:" << error;
    }
    emit queryHighScoresCompleted(success, operationResult, highScores);
}

void LeaderboardHelper::handleQueryHighScoreError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
}

void LeaderboardHelper::sendAppOpenedEvent()
{
    qDebug() << "sending AppOpened Event";
    QNetworkRequest request;
    configureStandardRequest(request, APP_OPENED_PATH);
    request.setHeader(QNetworkRequest::ContentTypeHeader, CONTENT_TYPE_JSON_HEADER_VALUE);
    mState = SendingAnalyticsInProgress;
    mReply = mNetworkManager->post(request, "{}");
    connectReplySignals();
}

void LeaderboardHelper::sendSettingsOpenedEvent()
{
    qDebug() << "sending settings visited Event";
    QNetworkRequest request;
    configureStandardRequest(request, SETTINGS_OPENED_PATH);
    request.setHeader(QNetworkRequest::ContentTypeHeader, CONTENT_TYPE_JSON_HEADER_VALUE);
    mState = SendingAnalyticsInProgress;
    SimpleJSONCreator *jsonCreator = new SimpleJSONCreator(this);
    JSONObject *object = new JSONObject();
    object->addValue(SETTINGS_OPENED_FIELD, "1");
    jsonCreator->addObject(DIMENSIONS_FIELD, object);
    mReply = mNetworkManager->post(request, jsonCreator->getJsonData());
    connectReplySignals();
    jsonCreator->deleteLater();
}
