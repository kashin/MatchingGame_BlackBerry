#ifndef SIMPLEJSONCREATOR_H_
#define SIMPLEJSONCREATOR_H_

#include <QObject>
#include <QList>
#include <QPair>
#include <QString>

class SimpleJSONCreator: public QObject
{
public:
    SimpleJSONCreator(QObject *parent = NULL);
    virtual ~SimpleJSONCreator();

    void addValue(const QString &name, const QString &value);
    QByteArray getJsonData() const;

private:
    QList<QPair<QString, QString> > mValues;
};

#endif /* SIMPLEJSONCREATOR_H_ */
