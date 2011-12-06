#include "config.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

QString Config::homeDir()
{
    return QDir::home().absolutePath() + QDir::separator() + ".roja-converter";
}

bool Config::checkConfig()
{
    QDir homeDir(QDir::home());
    if (!homeDir.cd(".roja-converter"))
    {
        if (!homeDir.mkdir(".roja-converter") || !homeDir.cd(".roja-converter"))
        {
            return false;
        }
    }

    return true;
}

QString Config::tempDir()
{
    return homeDir() + QDir::separator() + "Temp";
}

QString Config::databasePath()
{
    return homeDir() + "/" + "RojaDatabase.db.sqlite";
}
