#ifndef HIGHSCORE_H_
#define HIGHSCORE_H_

#include <QMetaType>

struct HighScore
{
    QString playerName;
    quint32 score;
    quint32 difficulty;
    quint32 level;
};

Q_DECLARE_METATYPE(HighScore)

#endif /* HIGHSCORE_H_ */
