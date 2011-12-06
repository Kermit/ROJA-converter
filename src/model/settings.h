#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QString>

class Settings
{
private:
    QString version;

public:
    Settings();

    void setVersion(QString value);
    QString getVersion();
};

#endif // SETTINGS_H
