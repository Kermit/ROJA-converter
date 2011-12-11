#include "routesdetails.h"

RoutesDetails::RoutesDetails()
{
    id = -1;
    routeID = -1;
    stopID = -1;
    lineID = -1;
}

void RoutesDetails::setID(int value)
{
    id = value;
}

int RoutesDetails::getID()
{
    return id;
}

void RoutesDetails::setRouteID(int value)
{
    routeID = value;
}

int RoutesDetails::getRouteID()
{
    return routeID;
}

void RoutesDetails::setStopID(int value)
{
    stopID = value;
}

int RoutesDetails::getStopID()
{
    return stopID;
}

void RoutesDetails::setLineID(int value)
{
    lineID = value;
}

int RoutesDetails::getLineID()
{
    return lineID;
}

void RoutesDetails::setRoutesDetailsID(int value)
{
    routesDetailsID = value;
}

int RoutesDetails::getRoutesDetailsID()
{
    return routesDetailsID;
}

QString RoutesDetails::toString()
{
    return QString("%1, %2, %3, %4").arg(id).arg(lineID).arg(stopID).arg(routeID);
}
