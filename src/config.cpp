#include "config.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

QString Config::homeDir()
{
    QDir homeDir(QDir::home());
    homeDir.cd(".roja-converter");

    return homeDir.absolutePath();
}

void Config::checkConfig()
{
    QDir homeDir(QDir::home());
    if (!homeDir.cd(".roja-converter"))
    {
        homeDir.mkdir(".roja-converter");
        homeDir.cd(".roja-converter");
    }
}
