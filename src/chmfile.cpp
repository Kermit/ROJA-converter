#include "chmfile.h"
#include "config.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QRegExp>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QDate>
#include <QtCore/QUrl>
#include <QtCore/QTextCodec>
#include <QtCore/QtAlgorithms>
#include <QtNetwork/QNetworkReply>

#include <iostream>

using namespace std;

/**
* Constructor that connects downloadManager signals
*/
CHMFile::CHMFile(QObject *parent) :
    QObject(parent)
{
    for (int i = 0; i < 7; ++i)
    {
        ids[i] = 1;
    }

    connect(&downloadManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(DownloadFinished(QNetworkReply*)));
}

/**
* Download page with timetable.
*
* From this page we'll get the newest file with timetable. It calls downloadManager.
*/
void CHMFile::startConvert()
{
    QUrl url("http://rozklady.kzkgop.pl/index.php?co=rozklady&submenu=pobierz");
    QNetworkRequest request(url);
    downloadManager.get(request);
}

/**
* Download handler.
*
* @param reply pointer to reply from HTTP request
*/
void CHMFile::DownloadFinished(QNetworkReply *reply)
{
    if (!reply->url().toString().contains(".zip"))
    {
        QString content = reply->readAll();
        QRegExp regexp("(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d).zip");
        if (regexp.indexIn(content) > -1)
        {
           filename = regexp.cap(1) + "-" + regexp.cap(2) + "-" + regexp.cap(3);
        }

        checkUpToDate();
    }
    else
    {
        cout << endl;

        QFile file(Config::homeDir() + "/" + filename + ".zip");
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(reply->readAll());
            file.close();
            convert();
        }
        else
        {
            //jakiś debug
            qApp->exit(0);
        }
    }
}

/**
* Shows download progress
*
* @param downloadedLenght   how much bytes we downloaded already
* @param totalLenght        how big is the content
*/
void CHMFile::DownloadProgress(qint64 downloadedLenght, qint64 totalLenght)
{
    int percent = (downloadedLenght * 100)/totalLenght;
    QString text = QString("Pobrano %1% (%2/%3)").
            arg(QString::number(percent), QString::number(downloadedLenght), QString::number(totalLenght));

    cout << text.toStdString() << '\xd';
}

/**
* Checks if we have the newest timetable file.
*
* If there is the newest file and database method do nothing. If file or database isn't fresh we download file/do convert/
* download file and do convert.
*/
void CHMFile::checkUpToDate()
{
    // If there isn't the newest ZIP file then download it and convert
    if (!QFile(Config::homeDir() + "/" + filename + ".zip").exists())
    {
        QNetworkRequest downloadFileRequest(QUrl("http://rozklady.kzkgop.pl/pobierz/" + filename + ".zip"));
        downloadFileReply = downloadManager.get(downloadFileRequest);
        connect(downloadFileReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(DownloadProgress(qint64,qint64)));
    }
    else //if there is the newest ZIP file
    {
        // TODO
        // If there isn't database file then convert it
        // If there is database file chceck version value.
        // If version value isn't equals to filename then convert else do nothing
        convert();
    }
}

/**
* Convert timetable.
*
* Converts timetable form ZIP file into SQLite database.
*/
void CHMFile::convert()
{
    //Check if old Temp dir exists, if yes delete it.
    if (QDir(Config::tempDir()).exists())
    {
        deleteDir(Config::tempDir());
    }

    unZipCHM();
    decompressCHM();
    cleanSource();

    database.createDatabase();

    // Start convert all dirs
    QDir sourceDir(Config::tempDir());
    foreach (QFileInfo fileInfo, sourceDir.entryInfoList((QDir::NoDotAndDotDot | QDir::System | QDir::Hidden |
                                                          QDir::AllDirs | QDir::Files), QDir::DirsFirst))
    {
        if (fileInfo.isDir())
        {
            convertDir(QDir(fileInfo.absoluteFilePath()));
        }
    }

    database.setDefaultSettings(filename);
    database.addToCommunesTable(communes);
    database.addToStopsTable(stops);
    database.addToLinesTable(lines);
    database.addToRoutesDetailsTable(routesDetails);
    cleanAfterConvert();
    qApp->exit(1);
}

void CHMFile::unZipCHM()
{
    QFile chmFile(Config::homeDir() + QDir::separator() + filename + ".chm");

    if (!chmFile.exists())
    {
        QString program("unzip");
        QStringList arguments;
        arguments << "-o";
        arguments << "-d" + Config::homeDir();
        arguments << Config::homeDir() + QDir::separator() + filename + ".zip";

        QProcess unzip(this);
        int result = unzip.execute(program, arguments);

        if (result == -2 || result == -1)
        {
            //debug
            qApp->exit(0);
        }
    }
}

void CHMFile::decompressCHM()
{
    QDir tempDir(Config::tempDir());

    if (!tempDir.exists())
    {
        QString program("extract_chmLib");
        QStringList arguments;
        arguments << Config::homeDir() + QDir::separator() + filename + ".chm" <<
                     Config::homeDir() + QDir::separator() + "Temp";

        QProcess decompress(this);
        int result = decompress.execute(program, arguments);

        if (result == -2 || result == -1)
        {
            //debug
            qApp->exit(0);
        }
    }
}

void CHMFile::cleanSource()
{
    QDir sourceDir(Config::tempDir());

    foreach (QFileInfo fileInfo, sourceDir.entryInfoList((QDir::NoDotAndDotDot | QDir::System | QDir::Hidden |
                                                          QDir::AllDirs | QDir::Files), QDir::DirsFirst))
    {
        if (fileInfo.isDir())
        {
            if (fileInfo.fileName() == "img" || fileInfo.fileName() == "style")
            {
                deleteDir(sourceDir.absolutePath() + QDir::separator() + fileInfo.fileName());
            }
            else
            {
                getActualTimetable(QDir(fileInfo.filePath()));
            }
        }

        if (fileInfo.fileName().at(0) == '$' || fileInfo.fileName().at(0) == '#' || fileInfo.fileName() == "index.html")
        {
            QFile tempFile(fileInfo.fileName());
            tempFile.remove(sourceDir.absoluteFilePath(fileInfo.fileName()));
        }
    }
}

void CHMFile::getActualTimetable(QDir directory)
{
    QDate nearestDate;
    QStringList trasyFiles = directory.entryList(QStringList("trasy*"));

    if (trasyFiles.count() > 1)
    {
        QDate nowDate = QDate::currentDate();
        trasyFiles.removeOne("trasy.html");
        trasyFiles.replaceInStrings("trasy", "");
        trasyFiles.replaceInStrings(".html", "");

        foreach (QString stringDate, trasyFiles)
        {
            QDate date = QDate::fromString(stringDate, "yyyy-MM-dd");
            if (nowDate > date && date > nearestDate)
            {
                nearestDate = date;
            }
        }

        // Delete "trasy.html" file.
        // We don't need this file in that case
        QFile(directory.path() + QDir::separator() + "trasy.html").remove();

        foreach(QString stringDate, trasyFiles)
        {
            if (stringDate != nearestDate.toString("yyyy-MM-dd"))
            {
                QFile routeFile(directory.path() + QDir::separator() + "trasy" + stringDate + ".html");
                if (routeFile.open(QIODevice::ReadOnly))
                {
                    QStringList filesToDelete;

                    QString html = routeFile.readAll();
                    QRegExp routeNumbersRE("style=\"font-weight:bold;\" ><a href=\"(\\d+)_(\\d+).html");
                    int regExpPos = 0;

                    while ((regExpPos = routeNumbersRE.indexIn(html, regExpPos)) != -1)
                    {
                        filesToDelete << routeNumbersRE.cap(2);
                        regExpPos += routeNumbersRE.matchedLength();
                    }

                    qDebug() << filesToDelete;
                    QFileInfoList files = directory.entryInfoList(QStringList("*.html"));
                    foreach (QFileInfo fileInfo, files)
                    {
                        foreach (QString str, filesToDelete)
                        {
                            if (fileInfo.fileName().contains(str))
                            {
                                QFile(fileInfo.filePath()).remove();
                            }
                        }
                    }

                    routeFile.close();
                }

                routeFile.remove();
            }
            else
            {
                QFile::rename(directory.path() + QDir::separator() + "trasy" + stringDate + ".html",
                              directory.path() + QDir::separator() + "trasy.html");
            }
        }

        qDebug() << nearestDate;
        qDebug() << directory.dirName();
        qDebug() << trasyFiles;
        qDebug() << "\n";
    }
}

bool CHMFile::deleteDir(const QString &dirName)
{
    bool result = true;
    QDir directory(dirName);

    if (directory.exists())
    {
        foreach (QFileInfo fileInfo, directory.entryInfoList((QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files), QDir::DirsFirst))
        {
            if (fileInfo.isDir())
            {
                result = deleteDir(fileInfo.absoluteFilePath());
            }
            else
            {
                result = QFile::remove(fileInfo.absoluteFilePath());
            }

            if (!result)
            {
                return result;
            }
        }

        result = directory.rmdir(dirName);
    }

    return result;
}

/**
* Converts all files from Line directory.
*
* Every Line have his own directory. Here we get into that directory and convert all HTML files.
* Sometimes in one folder are different routes for the same line. We convert only actual routes. We can identify them
* by looking at filename - "trasyYYYY-MM-DD.html". Date in filename says from what date that route will be used by this line.
* We need to find closet date to today.
*
* @param dir    line directory to convert
*/
void CHMFile::convertDir(QDir dir)
{
    Lines line;
    line.setID(ids[ELines]);
    line.setNumber(dir.dirName());
    lines.insert(line.getNumber(), line);

    QStringList fileList = dir.entryList((QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files), QDir::Name);
    qSort(fileList.begin(), fileList.end(), compareNames);

    QString routesFilePath = dir.absoluteFilePath("trasy.html");
    getCommunes(routesFilePath);
    getRoutes(routesFilePath, line);

    foreach (QString fileInfo, fileList)
    {

    }

    ++ids[ELines];
}

void CHMFile::getCommunes(QString filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString html = stream.readAll();
        QRegExp communeRegExp("gmina_(\\d+)\"></td><td> ([a-zA-ZęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.]+)");
        int regExpPos = 0;

        while ((regExpPos = communeRegExp.indexIn(html, regExpPos)) != -1)
        {
            if (!communes.contains(communeRegExp.cap(1).toInt()))
            {
                Communes commune;
                commune.setID(ids[ECommunes]);
                commune.setNumber(communeRegExp.cap(1).toInt());
                commune.setName(communeRegExp.cap(2));
                communes.insert(commune.getNumber(), commune);

                ++ids[ECommunes];
            }
            regExpPos += communeRegExp.matchedLength();
        }

        file.close();
    }
}

void CHMFile::getRoutes(QString filePath, Lines &line)
{
    uint type = 0;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");

        QRegExp leftRoute("<div id=\"lewo\">");
        QRegExp rightRoute("<div id=\"prawo\">");
        QRegExp middleRoute("<div id=\"srodek\">");
        QRegExp stopRE(QString("td_darr\\d? gmina_(\\d+)\"></td><td class=\" td_przystanek(_wariant_\\d)? \"") +
                       QString("( style=\"font-weight:bold;\" )?>") +
                       QString("<a href=\"(\\d+)_\\d\\d\\d\\d.html\">([a-zA-Z0-9ęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.\\*+-_]+)</a>"));

        while (!stream.atEnd())
        {
            QString htmlLine = stream.readLine();
            Stops stop;

            if (leftRoute.indexIn(htmlLine) != -1)
            {
                type = 1;
            }

            if (rightRoute.indexIn(htmlLine) != -1)
            {
                type = 2;
            }

            if (middleRoute.indexIn(htmlLine) != -1)
            {
                type = 3;
            }

            if (stopRE.indexIn(htmlLine) != -1)
            {
                if (!stops.contains(stopRE.cap(5)))
                {
                    Communes commune = communes.value(stopRE.cap(1).toInt());
                    stop.setID(ids[EStops]);
                    stop.setName(stopRE.cap(5));
                    stop.setCommuneID(commune.getID());

                    ++ids[EStops];
                    stops.insert(stop.getName(), stop);
                }
                else
                {
                    stop = stops.value(stopRE.cap(5));
                }
            }

            RoutesDetails routeDetails;
            routeDetails.setID(ids[ERoutesDetails]);
            routeDetails.setLineID(line.getID());
            routeDetails.setStopID(stop.getID());

            // Set parent if it's not first stop on that route.
            if (stopRE.cap(4).toInt() > 1)
            {
                RoutesDetails parent = routesDetails.value(ids[ERoutesDetails]--);
                parent.setRoutesDetailsID(routeDetails.getID());
            }
            else
            {
                if (type == 1)
                {
                    line.setRoute1ID(routeDetails.getID());
                }

                if (type == 2)
                {
                    line.setRoute2ID(routeDetails.getID());
                }

                if (type == 3)
                {
                    line.setRoute1ID(routeDetails.getID());
                    line.setRoute2ID(routeDetails.getID());
                }
            }

            routesDetails.insert(routeDetails.getID(), routeDetails);
            ++ids[ERoutesDetails];
        }
        file.close();
    }
}

bool CHMFile::compareNames(const QString &s1, const QString &s2)
{
    int n1 = s1.section("_", 0, 0).toInt();
    int n2 = s2.section("_", 0, 0).toInt();

    return (n1 < n2);
}

void CHMFile::cleanAfterConvert()
{
    QDir homeDir(Config::homeDir());

    deleteDir(Config::tempDir());
    homeDir.remove(filename + ".chm");
}
