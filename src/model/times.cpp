#include "times.h"

Times::Times()
{
    id = -1;
    stopID = -1;
    lineID = -1;
    dayID = -1;
    time = "";
    optional = false;
}

void Times::setID(int value)
{
    id = value;
}

int Times::getID()
{
    return id;
}

void Times::setStopID(int value)
{
    stopID = value;
}

int Times::getStopID()
{
    return stopID;
}

void Times::setLineID(int value)
{
    lineID = value;
}

int Times::getLineID()
{
    return lineID;
}

void Times::setDayID(int value)
{
    dayID = value;
}

int Times::getDayID()
{
    return dayID;
}

void Times::setTime(QString value)
{
    time = value;
}

QString Times::getTime()
{
    return time;
}

void Times::setOptional(bool value)
{
    optional = value;
}

bool Times::getOptional()
{
    return optional;
}
