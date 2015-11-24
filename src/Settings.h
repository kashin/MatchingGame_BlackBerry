#ifndef MATCHINGGAME_SETTINGS_H_
#define SETTINGS_H_

#include <QSettings>

class Settings: public QSettings
{
    Q_OBJECT
public:

    Q_ENUMS(Difficulty)

    enum Difficulty {
        EasyDifficulty = 3,
        NormalDifficulty = 5,
        HardDifficulty = 7
    };

    /**
     * Holds current difficulty level
     */
    Q_PROPERTY(int difficulty READ difficulty WRITE setDifficulty NOTIFY difficultyChanged )

    /**
     * Holds Player name
     */
    Q_PROPERTY(QString playerName READ playerName WRITE setPlayerName NOTIFY playerNameChanged )

public:
    Settings(QObject * parent = 0);
    virtual ~Settings();

    void setSessionToken(const QString &sessionToken);
    QString sessionToken() const;

public slots:
    void setValue(const QString & key, const QVariant & value);
    QVariant value(const QString & key, const QVariant & defaultValue = QVariant()) const;

    void setDifficulty(int newDifficulty);
    void setDifficulty(Difficulty newDifficulty);
    int difficulty();

    void setPlayerName(const QString &newPlayerName);
    QString playerName() const;
    QString defaulPlayerName() const;

signals:
    void difficultyChanged(int newDifficulty);
    void playerNameChanged(const QString &newPlayerName);
};

#endif // MATCHINGGAME_SETTINGS_H_
