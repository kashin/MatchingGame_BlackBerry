#include "Settings.h"

namespace {
static const char *DIFFICULTY_KEY = "difficulty";
static const char *SESSION_TOKEN = "sessionToken";
static const char *PLAYERNAME_KEY = "playerName";
static QLatin1String DEFAULT_PLAYERNAME("DefaultPlayer");
}

Settings::Settings(QObject * parent)
    : QSettings(parent)
{
}

Settings::~Settings()
{
}

void Settings::setValue(const QString& key, const QVariant& value)
{
    QSettings::setValue(key, value);
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const
{
    return QSettings::value(key, defaultValue);
}

void Settings::setDifficulty(int newDifficulty)
{
    if (newDifficulty != difficulty()) {
        setValue(DIFFICULTY_KEY, newDifficulty);
        emit difficultyChanged(newDifficulty);
    }
}

void Settings::setDifficulty(Difficulty newDifficulty)
{
    setDifficulty((int)newDifficulty);
}

int Settings::difficulty()
{
    return value(DIFFICULTY_KEY, NormalDifficulty).toInt();
}

void Settings::setSessionToken(const QString& sessionToken)
{
    setValue(SESSION_TOKEN, sessionToken);
}

QString Settings::sessionToken() const
{
    return value(SESSION_TOKEN, QString()).toString();
}

void Settings::setPlayerName(const QString& newPlayerName)
{
    if (playerName() != newPlayerName) {
        setValue(PLAYERNAME_KEY, newPlayerName);
        emit playerNameChanged(newPlayerName);
    }
}

QString Settings::playerName() const
{
    return value(PLAYERNAME_KEY, QString()).toString();
}

QString Settings::defaulPlayerName() const
{
    return DEFAULT_PLAYERNAME;
}
