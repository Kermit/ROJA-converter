#include "stops.h"

Stops::Stops()
{
    id = -1;
    name = "";
    communeID = -1;
}

void Stops::setID(int value)
{
    this->id = value;
}

int Stops::getID()
{
    return id;
}

void Stops::setName(QString value)
{
    this->name = value;
}

QString Stops::getName()
{
    return name;
}

void Stops::setCommuneID(int value)
{
    this->communeID = value;
}

int Stops::getCommuneID()
{
    return communeID;
}

QString Stops::toString()
{
     return QString("%1, %2, %3").arg(id).arg(name).arg(communeID);
}
