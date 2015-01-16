#ifndef CATEGORY_H
#define CATEGORY_H

#include <QtCore/QObject>

class Category
{
public:
    Category(QString id, QString name);

    QString getName() const { return m_name; }


private:
    const QString m_id;
    const QString m_name;
};

#endif // CATEGORY_H
