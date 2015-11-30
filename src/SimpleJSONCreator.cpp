#include "SimpleJSONCreator.h"
#include "JSONObject.h"

// Qt
#include <QDebug>

SimpleJSONCreator::SimpleJSONCreator(QObject *parent)
    : QObject(parent),
      mMainObject(new JSONObject(this))
{
}

SimpleJSONCreator::~SimpleJSONCreator()
{
}

void SimpleJSONCreator::addValue(const QString& name, const QString& value)
{
    mMainObject->addValue(name, value);
}

void SimpleJSONCreator::addObject(const QString& name, JSONObject* childObject)
{
    mMainObject->addObject(name, childObject);
}

void SimpleJSONCreator::addIntValue(const QString& name, int value)
{
    mMainObject->addIntValue(name, value);
}

QByteArray SimpleJSONCreator::getJsonData() const
{
    const QString &result = mMainObject->jsonData();
    qDebug() << "resulted JSON data:" << result;
    return result.toUtf8();
}
