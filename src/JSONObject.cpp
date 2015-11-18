#include <src/JSONObject.h>

#include <QDebug>

JSONObject::JSONObject(QObject *parent)
  : QObject(parent)
{
}

JSONObject::~JSONObject()
{
}

void JSONObject::addValue(const QString& name, const QString& value)
{
    mValues.append(qMakePair(name, value));
}

void JSONObject::addObject(const QString& name, JSONObject* childObject)
{
    childObject->setParent(this);
    mChildObjects.append(qMakePair(name, childObject));
}

QString JSONObject::jsonData() const
{
    QString result('{');
    bool first = true;
    typedef QPair<QString, QString> ValuePair;
    // first let's append values
    foreach (const ValuePair &value, mValues) {
        if (!first) {
            result.append(',');
        }
        result.append('"' + value.first + "\":\"" + value.second + '"');
        first = false;
    }
    // now let's append all child objects
    typedef QPair<QString, JSONObject*> ChildObjectPair;
    foreach(const ChildObjectPair &object, mChildObjects) {
        if (!object.second) {
            qCritical() << "child object is null!!!";
            continue;
        }
        if (!first) {
            result.append(',');
        }
        result.append('"' + object.first + "\":" + object.second->jsonData());
        first = false;
    }
    result.append('}');
    return result;
}

void JSONObject::parseJSONData(const QByteArray& data)
{
    const QString &dataStr = QString::fromUtf8(data.data(), data.length());
    qDebug() << "data for parsing:" << dataStr;
    if (dataStr.isEmpty() || dataStr.at(0) != '{') {
        qWarning() << "data is empty or invalid:" << dataStr;
        return;
    }
    QString restDataStr = dataStr.simplified();
    restDataStr.remove(0, 1).remove(' ');
    while(!restDataStr.isEmpty()) {
        // first let's get the name of the value
        QString valueName;
        int indexOfStart = restDataStr.indexOf('"');
        restDataStr.remove(0, 1);
        int indexOfEnd = restDataStr.indexOf('"');
        valueName = restDataStr.mid(indexOfStart,
                indexOfEnd - indexOfStart);
        restDataStr.remove(indexOfStart, indexOfEnd + 1);

        if (!restDataStr.startsWith(':')) {
            qCritical() << "JSON parsing issue! restDataStr" << restDataStr;
            break;
        }
        restDataStr.remove(0, 1);
        // Now we need to parse the value itself
        indexOfStart = 0;
        QString currentValueStr;
        if (restDataStr.startsWith('{')) {
            // ok, it's an object
            indexOfEnd = restDataStr.indexOf('}');
            currentValueStr = restDataStr.mid(0, indexOfEnd + 1);
            JSONObject *newObject = new JSONObject(this);
            newObject->parseJSONData(currentValueStr.toUtf8());
            mChildObjects.append(qMakePair(valueName, newObject));
            restDataStr.remove(0, indexOfEnd + 1);
        } else {
            if (restDataStr.startsWith('"')) {
                restDataStr.remove(0, 1);
                indexOfEnd = restDataStr.indexOf('"');
                currentValueStr = restDataStr.mid(0, indexOfEnd);
                mValues.append(qMakePair(valueName, currentValueStr));
                restDataStr.remove(0, indexOfEnd + 1); // remove including '"' character
            } else {
                indexOfEnd = restDataStr.indexOf(',');
                currentValueStr = restDataStr.mid(0, indexOfEnd);
                mValues.append(qMakePair(valueName, currentValueStr));
                restDataStr.remove(0, indexOfEnd);
            }
        }
        if (restDataStr.startsWith(',')) {
            restDataStr.remove(0, 1);
        } else if (restDataStr.startsWith('}')) {
            restDataStr.remove(0, 1);
            qDebug() << "end of object. RestData:" << restDataStr;
        } else {
            qCritical() << "something is parsed in a wrong way: RestData:" << restDataStr;
            break;
        }
    }
}
