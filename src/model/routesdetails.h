#ifndef ROUTESDETAILS_H
#define ROUTESDETAILS_H

class RoutesDetails
{
private:
    int id;
    int routeID;
    int stopID;
    int order;

public:
    RoutesDetails();

    void setID(int value);
    int getID();
    void setRouteID(int value);
    int getRouteID();
    void setStopID(int value);
    int getStopID();
    void setOrder(int value);
    int getOrder();
};

#endif // ROUTESDETAILS_H
