#include "SimpleJSONCreator.h"

// Qt
#include <QDebug>

SimpleJSONCreator::SimpleJSONCreator(QObject *parent)
    : QObject(parent)
{
}

SimpleJSONCreator::~SimpleJSONCreator()
{
}

void SimpleJSONCreator::addValue(const QString& name, const QString& value)
{
    mValues.append(qMakePair(name, value));
}

QByteArray SimpleJSONCreator::getJsonData() const
{
    QString result("{");
    bool first = true;
    typedef QPair<QString, QString> ValuePair;
    foreach (const ValuePair &value, mValues) {
        if (!first) {
            result.append(",");
        }
        result.append('"' + value.first + '"' + ":\"" + value.second + '"');
        first = false;
    }
    result.append('}');
    qDebug() << "resulted JSON data:" << result;
    return result.toUtf8();
}
