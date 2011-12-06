#include "days.h"

Day::Day()
{
    id = -1;
    number = -1;
    name = "";
}

void Day::setID(int value)
{
    id = value;
}

int Day::getID()
{
    return id;
}

void Day::setNumber(int value)
{
    number = value;
}

int Day::getNumber()
{
    return number;
}

void Day::setName(QString value)
{
    name = value;
}

QString Day::getName()
{
    return name;
}
