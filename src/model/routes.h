#ifndef ROUTES_H
#define ROUTES_H

#include <QtCore/QString>

class Routes
{
private:
    int id;
    int startID;
    int stopID;
    int lineID;

public:
    Routes();

    void setID(int value);
    int getID();
    void setStartID(int value);
    int getStartID();
    void setStopID(int value);
    int getStopID();
    void setLineID(int value);
    int getLineID();
    QString toString();
};

#endif // ROUTES_H
