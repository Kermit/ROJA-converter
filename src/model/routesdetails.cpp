#include "routesdetails.h"

RoutesDetails::RoutesDetails()
{
    id = -1;
    routeID = -1;
    stopID = -1;
    lineID = -1;
    routesDetailsID = -1;
    onDemand = 0;
    obligatory = 0;
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

void RoutesDetails::setOnDemand(int value)
{
    onDemand = value;
}

int RoutesDetails::getOnDemand()
{
    return onDemand;
}

void RoutesDetails::setObligatory(int value)
{
    obligatory = value;
}

int RoutesDetails::getObligatory()
{
    return obligatory;
}

QString RoutesDetails::toString()
{
    return QString("%1, %2, %3, %4, %5, %6, %7").arg(id).arg(routeID).arg(lineID).arg(stopID).arg(routesDetailsID).arg(onDemand).arg(obligatory);
}
