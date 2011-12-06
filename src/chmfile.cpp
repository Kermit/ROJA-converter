#include "chmfile.h"
#include "config.h"

#include <QtCore/QCoreApplication>
#include <QUrl>
#include <QNetworkReply>
#include <QtCore/QRegExp>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>

#include <QtAlgorithms>

#include <iostream>

using namespace std;

/**
* Constructor that connects downloadManager signals
*/
CHMFile::CHMFile(QObject *parent) :
    QObject(parent)
{
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

    //Wydzielić do klasy database
    createDatabase();
    setDefaultSettings();

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
       if (fileInfo.fileName() == "img" || fileInfo.fileName() == "style")
       {
           deleteDir(sourceDir.absolutePath() + QDir::separator() + fileInfo.fileName());
        }

       if (fileInfo.fileName().at(0) == '$' || fileInfo.fileName().at(0) == '#' || fileInfo.fileName() == "index.html")
       {
           QFile tempFile(fileInfo.fileName());
           tempFile.remove(sourceDir.absoluteFilePath(fileInfo.fileName()));
       }
    }
}

void CHMFile::createDatabase()
{
    rojaDatabase = QSqlDatabase::addDatabase("QSQLITE");
    rojaDatabase.setDatabaseName(Config::databasePath());
    if (rojaDatabase.open())
    {
        QSqlQuery settingsTable;
        settingsTable.exec("CREATE TABLE settings"
                           "(id integer primary key, "
                           "name varchar(20),"
                           "value varchar(10))");
    }
}

void CHMFile::setDefaultSettings()
{
    QSqlTableModel settingsTable(this, rojaDatabase);
    settingsTable.setTable("settings");
    settingsTable.setEditStrategy(QSqlTableModel::OnManualSubmit);

    QSqlRecord settingsRecord = settingsTable.record();
    settingsRecord.setNull("id");
    settingsRecord.setValue("name", "version");
    settingsRecord.setValue("value", filename);

    settingsTable.insertRecord(-1,settingsRecord);
    settingsTable.submitAll();
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
    QStringList fileList = dir.entryList((QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files), QDir::Name);
    qSort(fileList.begin(), fileList.end(), compareNames);

    foreach (QString fileInfo, fileList)
    {
        cout << fileInfo.toStdString() << endl;
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
