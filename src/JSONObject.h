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

    void addIntValue(const QString &name, int value);

    /**
     * @Note This object takes ownership of the childObject after this call
     */
    void addObject(const QString &name, JSONObject* childObject);

    /**
     * @Note This object takes ownership of the childObject after this call
     */
    void addArrayObject(const QString &name, QList<JSONObject*> childObjects);

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

    QList<QPair<QString, QList<JSONObject*> > > childArrayObjects() const {
        return mChildArrayObjects;
    }

    const QList<QPair<QString, int> >& intValues() const
    {
        return mIntValues;
    }

private:
    QList<QPair<QString, int> > mIntValues;
    QList<QPair<QString, QString> > mValues;
    QList<QPair<QString, JSONObject*> > mChildObjects;
    QList<QPair<QString, QList<JSONObject*> > > mChildArrayObjects;
};

#endif /* JSONOBJECT_H_ */
