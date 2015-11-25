#include "LeaderBoardModel.h"

#include "LeaderboardHelper.h"

using namespace bb::cascades;

namespace {
    static const quint32 DEFAULT_LIMIT = 10;
}

LeaderBoardModel::LeaderBoardModel(QObject *parent)
    : DataModel(parent),
      mLimit(DEFAULT_LIMIT),
      mHelper(new LeaderboardHelper(this))
{
    connect(mHelper, SIGNAL(queryHighScoresCompleted(bool, int, QList<HighScore>)),
            this, SLOT(onHighScoreCompleted(bool, int, QList<HighScore>)));
}

LeaderBoardModel::~LeaderBoardModel()
{
}

int LeaderBoardModel::childCount(const QVariantList& indexPath)
{
    Q_UNUSED(indexPath);
    // TODO: implement
    return 0;
}

QVariant LeaderBoardModel::data(const QVariantList& indexPath)
{
    Q_UNUSED(indexPath);
    // TODO: implement
    return QVariant();
}

bool LeaderBoardModel::hasChildren(const QVariantList& indexPath)
{
    Q_UNUSED(indexPath);
    // TODO: implement
    return false;
}

void LeaderBoardModel::init()
{
    mHelper->queryHighScores(mLimit);
}

void LeaderBoardModel::onHighScoreCompleted(bool success, int error, QList<HighScore> highScores)
{
    qDebug() << "success:" << success << "error" << error;
    qDebug() << highScores.count();
    mHighScores = highScores;
}

quint32 LeaderBoardModel::limit() const
{
    return mLimit;
}

void LeaderBoardModel::setLimit(quint32 newLimit)
{
    if (mLimit != newLimit) {
        mLimit = newLimit;
        emit limitChanged(newLimit);
    }
}
