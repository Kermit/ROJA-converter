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
#include <QtCore/QByteArray>
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

        QFile databaseFile(Config::databasePath());
        if (databaseFile.exists())
        {
            QString databaseVersion = database.getDatabaseVersion();
        }
        else
        {
            convert();
        }
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

    if (database.openDatabase())
    {
        database.createDatabase();

        // Start convert all dirs
        QDir sourceDir(Config::tempDir());
        foreach (QFileInfo fileInfo, sourceDir.entryInfoList((QDir::NoDotAndDotDot | QDir::System | QDir::Hidden |
                                                              QDir::AllDirs | QDir::Files), QDir::Name | QDir::LocaleAware))
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
        database.addToRoutesTable(routes);
        database.addToRoutesDetailsTable(routesDetails);
        database.addToDaysTable(days);
        database.addToTimesTable(times);
        database.closeDatabase();

        QFile databaseFile(Config::databasePath());
        if (databaseFile.open(QIODevice::ReadOnly))
        {
            QByteArray compressed = qCompress(databaseFile.readAll(), 9);

            QFile databaseFileCompressed(Config::databasePath() + ".zip");
            if (databaseFileCompressed.open(QIODevice::WriteOnly))
            {
                databaseFileCompressed.write(compressed);
                databaseFileCompressed.close();
            }
            databaseFile.close();
        }
        cleanAfterConvert();
    }
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

    cout << line.getNumber().toStdString() << endl;
    QStringList fileList = dir.entryList((QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files), QDir::Name);
    qSort(fileList.begin(), fileList.end(), compareNames);

    int route1 = -1;
    int route2 = -1;

    QString routesFilePath = dir.absoluteFilePath("trasy.html");
    getCommunes(routesFilePath);
    getRoutes(routesFilePath, line, route1, route2);
    dir.remove(routesFilePath);

    foreach (QString fileInfo, fileList)
    {
        getTimes(dir.absoluteFilePath(fileInfo), line, route1, route2);
    }

    ++ids[ELines];
    lines.insert(line.getNumber(), line);
}

void CHMFile::getCommunes(QString filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString html = stream.readAll();
        QRegExp communeRegExp("gmina_(\\d+)\"></td><td> ([a-zA-ZęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.()]+)");
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

void CHMFile::getRoutes(QString filePath, Lines &line, int &route1, int &route2)
{
    uint onDemand = 0;

    Routes route;

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
                       QString("<a href=\"(\\d+)_\\d\\d\\d\\d.html\">([a-zA-Z0-9ęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.\\*+-_()]+)</a>"));
        QRegExp wayRE("Kierunek: <b>([a-zA-Z0-9ęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.\\*+-_()]+)</b>");

        while (!stream.atEnd())
        {
            QString htmlLine = stream.readLine();
            Stops stop;

            if (leftRoute.indexIn(htmlLine) != -1)
            {
                if (wayRE.indexIn(htmlLine) != -1)
                {
                    Stops wayStop;
                    if (!stops.contains(wayRE.cap(1)))
                    {
                        wayStop.setID(ids[EStops]);
                        wayStop.setName(wayRE.cap(1));
                        ++ids[EStops];
                        stops.insert(wayStop.getName(), wayStop);
                    }
                    else
                    {
                        wayStop = stops.value(wayRE.cap(1));
                    }


                    route.setID(ids[ERoutes]);
                    route.setLineID(line.getID());
                    route.setStopID(wayStop.getID());
                    ++ids[ERoutes];
                    routes.insert(route.getID(), route);

                    line.setRoute1ID(route.getID());
                    route1 = route.getID();
                }
            }

            if (rightRoute.indexIn(htmlLine) != -1)
            {
                if (wayRE.indexIn(htmlLine) != -1)
                {
                    Stops wayStop;
                    if (!stops.contains(wayRE.cap(1)))
                    {
                        wayStop.setID(ids[EStops]);
                        wayStop.setName(wayRE.cap(1));
                        ++ids[EStops];
                        stops.insert(wayStop.getName(), wayStop);
                    }
                    else
                    {
                        wayStop = stops.value(wayRE.cap(1));
                    }


                    route.setID(ids[ERoutes]);
                    route.setLineID(line.getID());
                    route.setStopID(wayStop.getID());
                    ++ids[ERoutes];
                    routes.insert(route.getID(), route);

                    line.setRoute2ID(route.getID());
                    route2 = route.getID();
                }
            }

            if (middleRoute.indexIn(htmlLine) != -1)
            {
                if (wayRE.indexIn(htmlLine) != -1)
                {
                    Stops wayStop;
                    if (!stops.contains(wayRE.cap(1)))
                    {
                        wayStop.setID(ids[EStops]);
                        wayStop.setName(wayRE.cap(1));
                        ++ids[EStops];
                        stops.insert(wayStop.getName(), wayStop);
                    }
                    else
                    {
                        wayStop = stops.value(wayRE.cap(1));
                    }


                    route.setID(ids[ERoutes]);
                    route.setLineID(line.getID());
                    route.setStopID(wayStop.getID());
                    ++ids[ERoutes];
                    routes.insert(route.getID(), route);

                    line.setRoute1ID(route.getID());
                    line.setRoute2ID(route.getID());
                    route1 = route.getID();
                    route2 = route.getID();
                }
            }

            if (stopRE.indexIn(htmlLine) != -1)
            {
                QString stopName = stopRE.cap(5);
                if (stopName.contains("Gojny"))
                {
                    //int ss = 0;
                }
                if (stopName.contains(" <b>n/ż</b>"))
                {
                    onDemand = 1;
                    stopName = stopName.remove(" <b>n/ż</b>");
                }
                else
                {
                    onDemand = 0;
                }

                if (!stops.contains(stopName))
                {
                    Communes commune = communes.value(stopRE.cap(1).toInt());
                    stop.setID(ids[EStops]);
                    stop.setName(stopName);
                    stop.setCommuneID(commune.getID());

                    ++ids[EStops];
                    stops.insert(stop.getName(), stop);
                }
                else
                {
                    stop = stops.value(stopName);
                    if (stop.getCommuneID() == -1)
                    {
                        Communes commune = communes.value(stopRE.cap(1).toInt());
                        stop.setCommuneID(commune.getID());
                        stops.insert(stop.getName(), stop);
                    }
                }

                QString obligatoryStop = stopRE.cap(2);

                RoutesDetails routeDetails;
                routeDetails.setID(ids[ERoutesDetails]);
                routeDetails.setLineID(line.getID());
                routeDetails.setStopID(stop.getID());
                routeDetails.setRouteID(route.getID());
                routeDetails.setOnDemand(onDemand);
                if (!obligatoryStop.isEmpty())
                {
                    routeDetails.setObligatory(1);
                }
                else
                {
                    routeDetails.setObligatory(0);
                }

                // Set parent if it's not first stop on that route.
                if (stopRE.cap(4).toInt() > 1)
                {
                    RoutesDetails parent = routesDetails.value(ids[ERoutesDetails]-1);
                    parent.setRoutesDetailsID(routeDetails.getID());
                    routesDetails.insert(parent.getID(), parent);
                }

                routesDetails.insert(routeDetails.getID(), routeDetails);
                ++ids[ERoutesDetails];
            }
        }
        file.close();
    }
}

void CHMFile::getTimes(QString filePath, Lines &line, int route1, int route2)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        Routes routes1 = routes.value(route1);
        Routes routes2 = routes.value(route2);
        QString hour;
        Days day;
        QString currentStopName;
        QString wayStopName;
        QString legendStr;

        QTextStream stream(&file);
        stream.setCodec("UTF-8");

        while (!stream.atEnd())
        {
            QRegExp stopR("<h2>Przystanek: <a href=\"#\" > ([a-zA-Z0-9ęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.\\*+-_()]+)</a> ");
            QRegExp wayStopR("<h3>Kierunek: ([a-zA-Z0-9ęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.\\*+-_()]+)</h3>");
            QRegExp wayStopSR("<b>([a-zA-Z0-9ęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.\\*+-_()]+)</b>jest");
            QRegExp dayType("(\\d+)\"><th>([a-zA-Z0-9ęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.\\*+-_]+)</th></tr>");
            QRegExp hourRE("<b>(\\d+)</b>");
            QRegExp minutesRE("<sup class=\"typ_(\\d)\"( )?>(\\d+)"
                              "(<span class=\"span_literka\">([a-zA-Z0-9ęóąśłżźćńĘÓĄŚŁŻŹĆŃ\\s\\.\\*+-_]+)</span>)?</sup>");
            QRegExp legendRE("<table class=\"legenda_literki\"><tr>");

            QString htmlLine = stream.readLine();
            QStringList list = htmlLine.split("<tr class=\"typ_dnia_");

            if (legendRE.indexIn(htmlLine) != -1)
            {
                legendStr = htmlLine;
                int posStart = legendStr.indexOf("<table class=\"legenda_literki\">");
                legendStr = legendStr.remove(0, posStart);
                int posStop = legendStr.indexOf("</tr></table>");
                legendStr = legendStr.remove(posStop, legendStr.size());
                legendStr = legendStr.replace("</td><td><b>", " ");
                legendStr = legendStr.replace("<table class=\"legenda_literki\">", "");
                legendStr = legendStr.replace(" &raquo; ", ", ");
                legendStr = legendStr.replace("<span class='kurs_przez'>", "");
                legendStr = legendStr.replace("<b>", "");
                legendStr = legendStr.replace("</b>", "");
                legendStr = legendStr.replace("<span>", "");
                legendStr = legendStr.replace("</span>", "");
                legendStr = legendStr.replace("<td>", "");
                legendStr = legendStr.replace("</td>", "");
                legendStr = legendStr.replace("<tr>", "");
                legendStr = legendStr.replace("</tr>", "");
                legendStr = legendStr.replace("<td class='span_literka'>", ";");
            }

            if (htmlLine.contains("<h2>Przystanek: "))
            {
                stopR.indexIn(htmlLine);
                currentStopName = stopR.cap(1);
                if (currentStopName.contains(" n/ż"))
                {
                    currentStopName = currentStopName.remove(" n/ż");
                }
            }

            if (htmlLine.contains("<b>"))
            {
                if (wayStopR.indexIn(htmlLine) != -1)
                {
                    wayStopName = wayStopR.cap(1);
                }

                if (wayStopSR.indexIn(htmlLine) != -1)
                {
                    wayStopName = wayStopSR.cap(1);
                }

                if (wayStopName.contains(" n/ż"))
                {
                    wayStopName = wayStopName.remove(" n/ż");
                }
            }

            if (list.count() > 1)
            {
                Stops currentStop = stops.value(currentStopName);
                if (currentStop.getID() == -1)
                {
                    //int ss =0;
                }
                Stops wayStop = stops.value(wayStopName);
                if (wayStop.getID() == -1)
                {
                    //int ss =0;
                }
                Routes currentRoute;
                if (routes1.getStopID() == wayStop.getID())
                {
                    currentRoute = routes1;
                }
                else
                {
                    currentRoute = routes2;
                }

                bool ok = true;
                for (int i = 1; i < list.count(); ++i)
                {
                    if (dayType.indexIn(list[i]) != -1)
                    {
                        if (!days.contains(dayType.cap(2)))
                        {
                            day.setID(ids[EDays]);
                            day.setName(dayType.cap(2));
                            day.setNumber(dayType.cap(1).toInt());

                            days.insert(day.getName(), day);
                            ++ids[EDays];
                        }
                        else
                        {
                            day = days.value(dayType.cap(2));
                        }
                    }

                    QStringList hoursList = list[i].split("<span id=\"blok_godzina\">");

                    QString timeS = "";
                    for (int hIndex = 1; hIndex < hoursList.count(); ++hIndex)
                    {
                        hourRE.indexIn(hoursList[hIndex]);
                        hour = hourRE.cap(1);

                        int regExpPos = 0;

                        QString minutes = "";
                        while ((regExpPos = minutesRE.indexIn(hoursList[hIndex], regExpPos)) != -1)
                        {
                            minutes += minutesRE.cap(3) + minutesRE.cap(5) + ";";
                            regExpPos += minutesRE.matchedLength();
                        }

                        minutes = minutes.remove(minutes.lastIndexOf(";"), 1);
                        timeS += hour + ":" + minutes + ",";
                    }

                    timeS = timeS.remove(timeS.lastIndexOf(","), 1);

                    RoutesDetails currentRD;
                    bool stopSearch = false;
                    foreach (RoutesDetails rd, routesDetails)
                    {
                        if (rd.getLineID() == line.getID() && rd.getStopID() == currentStop.getID()
                                && rd.getRouteID() == currentRoute.getID())
                        {
                            foreach (Times t, times.values(rd.getID()))
                            {
                                if (t.getDayID() == day.getID())
                                {
                                    stopSearch = true;
                                    break;
                                }
                            }

                            if (!stopSearch)
                            {
                                currentRD = rd;
                                break;
                            }

                            if (stopSearch)
                            {
                                stopSearch = false;
                            }
                        }
                    }

                    if (currentRD.getID() == -1)
                    {
                        //int s = 0;
                    }

                    Times time;
                    time.setID(ids[ETimes]);
                    //time.setLineID(line.getID());
                    //time.setStopID(currentStop.getID());
                    time.setDayID(day.getID());
                    time.setRouteDetailsID(currentRD.getID());
                    //time.setRouteID(currentRoute.getID());
                    time.setTime(timeS);
                    if (ok)
                    {
                        time.setLegend(legendStr);
                        ok = false;
                    }

                    ++ids[ETimes];
                    times.insert(currentRD.getID(), time);
                }
            }
        }
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
