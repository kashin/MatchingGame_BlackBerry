#ifndef SIMPLEJSONCREATOR_H_
#define SIMPLEJSONCREATOR_H_

#include <QObject>
#include <QList>
#include <QPair>
#include <QString>

class JSONObject;

class SimpleJSONCreator: public QObject
{
public:
    SimpleJSONCreator(QObject *parent = NULL);
    virtual ~SimpleJSONCreator();

    void addObject(const QString &name, JSONObject *childObject);
    void addValue(const QString &name, const QString &value);
    void addIntValue(const QString &name, int value);
    QByteArray getJsonData() const;

private:
    JSONObject *mMainObject;
    QList<QPair<QString, JSONObject*> > mChildObjects;
};

#endif /* SIMPLEJSONCREATOR_H_ */
