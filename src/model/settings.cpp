#include "settings.h"

Settings::Settings()
{
    version = "";
}

void Settings::setVersion(QString value)
{
    version = value;
}

QString Settings::getVersion()
{
    return version;
}
