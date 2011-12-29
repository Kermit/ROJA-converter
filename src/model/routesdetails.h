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
    int onDemand;
    int obligatory;

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
    void setOnDemand(int value);
    int getOnDemand();
    void setObligatory(int value);
    int getObligatory();
    QString toString();
};

#endif // ROUTESDETAILS_H
