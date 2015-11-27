#ifndef LEADERBOARDMODEL_H_
#define LEADERBOARDMODEL_H_

#include <bb/cascades/DataModel>
#include "HighScore.h"

// Qt
#include <QList>

class LeaderboardHelper;

class LeaderBoardModel: public bb::cascades::DataModel
{
    Q_OBJECT
public:
        Q_PROPERTY( quint32 limit READ limit WRITE setLimit NOTIFY limitChanged  )

    LeaderBoardModel(QObject *parent = NULL);
    virtual ~LeaderBoardModel();

public:
    Q_INVOKABLE int childCount(const QVariantList &indexPath);
    Q_INVOKABLE QVariant data(const QVariantList &indexPath);
    Q_INVOKABLE bool hasChildren (const QVariantList &indexPath);
    Q_INVOKABLE QString itemType(const QVariantList& indexPath);

public slots:
    void init();

    void onHighScoreCompleted(bool success, int error, QList<HighScore> highScores);

    quint32 limit() const;
    void setLimit(quint32 newLimit);

signals:
    void limitChanged(quint32 newLimit);

private:

private:
    quint32 mLimit;
    LeaderboardHelper *mHelper;
    QList<HighScore> mHighScores;
};

#endif /* LEADERBOARDMODEL_H_ */
