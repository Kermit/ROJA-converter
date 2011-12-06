#include "lines.h"

Lines::Lines()
{
    id = -1;
    number = "";
    route1ID = -1;
    route2ID = -1;
}

void Lines::setID(int value)
{
    id = value;
}

int Lines::getID()
{
    return id;
}

void Lines::setNumber(QString value)
{
    number = value;
}

QString Lines::getNumber()
{
    return number;
}

void Lines::setRoute1ID(int value)
{
    route1ID = value;
}

int Lines::getRoute1ID()
{
    return route1ID;
}

void Lines::setRoute2ID(int value)
{
    route2ID = value;
}

int Lines::getRoute2ID()
{
    return route2ID;
}
