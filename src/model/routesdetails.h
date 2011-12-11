#ifndef ROUTESDETAILS_H
#define ROUTESDETAILS_H

#include <QtCore/QString>

class RoutesDetails
{
private:
    int id;
    int routeID;
    int stopID;
    int lineID;
    int routesDetailsID;

public:
    RoutesDetails();

    void setID(int value);
    int getID();
    void setRouteID(int value);
    int getRouteID();
    void setStopID(int value);
    int getStopID();
    void setLineID(int value);
    int getLineID();
    void setRoutesDetailsID(int value);
    int getRoutesDetailsID();
    QString toString();
};

#endif // ROUTESDETAILS_H
