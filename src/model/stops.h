#ifndef STOPS_H
#define STOPS_H

#include <QtCore/QString>

class Stops
{
private:
    int id;
    QString name;
    int communeID;

public:
    Stops();

    void setID(int value);
    int getID();
    void setName(QString value);
    QString getName();
    void setCommuneID(int value);
    int getCommuneID();
};

#endif // STOPS_H
