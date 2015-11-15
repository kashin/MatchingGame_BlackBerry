#include "Settings.h"

namespace {
static const char* DIFFICULTY_KEY = "difficulty";
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
