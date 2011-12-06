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
    QString time;
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
    void setTime(QString value);
    QString getTime();
    void setOptional(bool value);
    bool getOptional();
};

#endif // TIMES_H
