#include "days.h"

Days::Days()
{
    id = -1;
    number = -1;
    name = "";
}

void Days::setID(int value)
{
    id = value;
}

int Days::getID()
{
    return id;
}

void Days::setNumber(int value)
{
    number = value;
}

int Days::getNumber()
{
    return number;
}

void Days::setName(QString value)
{
    name = value;
}

QString Days::getName()
{
    return name;
}

QString Days::toString()
{
    return QString("%1, %2, %3").arg(id).arg(number).arg(name);
}
