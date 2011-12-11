#ifndef DAYS_H
#define DAYS_H

#include <QtCore/QString>

class Days
{
private:
    int id;
    int number;
    QString name;

public:
    Days();

    void setID(int value);
    int getID();
    void setNumber(int value);
    int getNumber();
    void setName(QString value);
    QString getName();
};

#endif // DAYS_H
