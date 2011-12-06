#include "routes.h"

Routes::Routes()
{
    id = -1;
    startID = -1;
    stopID = -1;
    lineID = -1;
}

void Routes::setID(int value)
{
    id = value;
}

int Routes::getID()
{
    return id;
}

void Routes::setStartID(int value)
{
    startID = value;
}

int Routes::getStartID()
{
    return startID;
}

void Routes::setStopID(int value)
{
    stopID = value;
}

int Routes::getStopID()
{
    return stopID;
}

void Routes::setLineID(int value)
{
    lineID = value;
}

int Routes::getLineID()
{
    return lineID;
}
