#ifndef COMMUNES_H
#define COMMUNES_H

#include <QtCore/QString>

class Communes
{
private:
    int id;
    int number;
    QString name;

public:
    Communes();

    void setID(int value);
    int getID();
    void setNumber(int value);
    int getNumber();
    void setName(QString value);
    QString getName();
};

#endif // COMMUNES_H
