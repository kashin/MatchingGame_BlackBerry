#ifndef JSONOBJECT_H_
#define JSONOBJECT_H_

#include <QObject>
#include <QList>
#include <QPair>
#include <QString>

class JSONObject: public QObject
{
public:
    JSONObject(QObject *parent = NULL);
    virtual ~JSONObject();

    void addValue(const QString &name, const QString &value);

    /**
     * This object takes ownership of the childObject after this call
     */
    void addObject(const QString &name, JSONObject* childObject);

    /**
     * Returns the resulted JSON data for this object
     */
    QString jsonData() const;

    void parseJSONData(const QByteArray &data);

    QList<QPair<QString, QString> > values() const {
        return mValues;
    }

    QList<QPair<QString, JSONObject*> > childObjects() const {
        return mChildObjects;
    }

private:
    QList<QPair<QString, QString> > mValues;
    QList<QPair<QString, JSONObject*> > mChildObjects;
};

#endif /* JSONOBJECT_H_ */
