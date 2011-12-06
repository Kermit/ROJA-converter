#include "communes.h"

Communes::Communes()
{
    id = -1;
    number = -1;
    name = "";
}

void Communes::setID(int value)
{
    id = value;
}

int Communes::getID()
{
    return id;
}

void Communes::setNumber(int value)
{
    number = value;
}

int Communes::getNumber()
{
    return number;
}

void Communes::setName(QString value)
{
    name = value;
}

QString Communes::getName()
{
    return name;
}
