#ifndef DAYS_H
#define DAYS_H

#include <QtCore/QString>

class Day
{
private:
    int id;
    int number;
    QString name;

public:
    Day();

    void setID(int value);
    int getID();
    void setNumber(int value);
    int getNumber();
    void setName(QString value);
    QString getName();
};

#endif // DAYS_H
