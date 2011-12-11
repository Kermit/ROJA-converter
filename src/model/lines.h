#ifndef LINES_H
#define LINES_H

#include <QtCore/QString>

class Lines
{
private:
    int id;
    QString number;
    int route1ID;
    int route2ID;

public:
    Lines();

    void setID(int value);
    int getID();
    void setNumber(QString value);
    QString getNumber();
    void setRoute1ID(int value);
    int getRoute1ID();
    void setRoute2ID(int value);
    int getRoute2ID();
    QString toString();
};

#endif // LINES_H
