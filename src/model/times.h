#ifndef TIMES_H
#define TIMES_H

#include <QtCore/QString>

class Times
{
private:
    int id;
    int stopID;
    int lineID;
    int dayID;
    int routeID;
    int routeDetailsID;
    QString time;
    QString legend;
    bool optional;

public:
    Times();

    void setID(int value);
    int getID();

    void setStopID(int value);
    int getStopID();
    void setLineID(int value);
    int getLineID();
    void setDayID(int value);
    int getDayID();
    void setRouteID(int value);
    int getRouteID();
    void setRouteDetailsID(int value);
    int getRouteDetailsID();
    void setTime(QString value);
    QString getTime();
    void setLegend(QString value);
    QString getLegend();
    void setOptional(bool value);
    bool getOptional();
    QString toString();
};

#endif // TIMES_H
