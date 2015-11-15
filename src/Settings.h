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

public:
    Settings(QObject * parent = 0);
    virtual ~Settings();

public slots:
    void setValue(const QString & key, const QVariant & value);
    QVariant value(const QString & key, const QVariant & defaultValue = QVariant()) const;

    void setDifficulty(int newDifficulty);
    void setDifficulty(Difficulty newDifficulty);
    int difficulty();

signals:
    void difficultyChanged(int newDifficulty);
};

#endif // MATCHINGGAME_SETTINGS_H_
