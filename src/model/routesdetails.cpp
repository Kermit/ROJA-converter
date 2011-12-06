#include "routesdetails.h"

RoutesDetails::RoutesDetails()
{
    id = -1;
    routeID = -1;
    stopID = -1;
    order = -1;
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

void RoutesDetails::setOrder(int value)
{
    order = value;
}

int RoutesDetails::getOrder()
{
    return order;
}
