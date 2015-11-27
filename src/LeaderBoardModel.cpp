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
    return mHighScores.count();
}

QVariant LeaderBoardModel::data(const QVariantList& indexPath)
{
    QVariantMap result;
    if (!indexPath.isEmpty()) {
        const QVariant &scoreIndex = indexPath.first();
        if (scoreIndex.isValid() && scoreIndex.canConvert<int>()) {
            int index = scoreIndex.toInt();
            if (index >= 0 && index < mHighScores.size()) {
                result["playerName"] = mHighScores.at(index).playerName;
                result["score"] = mHighScores.at(index).score;
                result["difficulty"] = mHighScores.at(index).difficulty;
                result["level"] = mHighScores.at(index).level;
                result["place"] = index + 1;
            }
        }
    }
    return result;
}

bool LeaderBoardModel::hasChildren(const QVariantList& indexPath)
{
    Q_UNUSED(indexPath);
    return false;
}

QString LeaderBoardModel::itemType(const QVariantList& indexPath)
{
    Q_UNUSED(indexPath);
    return "item";
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
    emit itemsChanged();
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
