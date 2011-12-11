#ifndef CHMFILE_H
#define CHMFILE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QHash>
#include <QtNetwork/QNetworkAccessManager>

#include "src/database.h"
#include "src/model/lines.h"
#include "src/model/stops.h"
#include "src/model/routes.h"
#include "src/model/days.h"
#include "src/model/communes.h"
#include "src/model/routesdetails.h"

class QNetworkReply;

class CHMFile : public QObject
{
    Q_OBJECT

private:
    enum
    {
        ELines = 0,
        ERoutes,
        EStops,
        ECommunes,
        EDays,
        ERoutesDetails,
        ETimes
    };

    Database database;
    QNetworkAccessManager downloadManager;
    QString filename;
    QNetworkReply *downloadFileReply;

    int ids[7];
    QHash<QString, Lines> lines;
    QHash<QString, Stops> stops;
    QHash<QString, Days> days;
    QHash<int, Communes> communes;
    QHash<int, RoutesDetails> routesDetails;

    void convert();
    void unZipCHM();
    void decompressCHM();
    void cleanSource();    
    void cleanAfterConvert();

    void getActualTimetable(QDir directory);
    void convertDir(QDir dir);
    void getCommunes(QString filePath);
    void getRoutes(QString filePath, Lines &line);

    bool deleteDir(const QString &dirName);
    static bool compareNames(const QString &s1, const QString &s2);

private slots:
    void checkUpToDate();

public:
    explicit CHMFile(QObject *parent = 0);
    void startConvert();

public slots:
    void DownloadFinished(QNetworkReply *reply);
    void DownloadProgress(qint64 downloadedLenght, qint64 totalLenght);

signals:
    void filenameDownloaded();
};

#endif // CHMFILE_H
