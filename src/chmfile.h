#ifndef CHMFILE_H
#define CHMFILE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtNetwork/QNetworkAccessManager>
#include <QtSql/QSqlDatabase>

class QNetworkReply;

class CHMFile : public QObject
{
    Q_OBJECT

private:
    QNetworkAccessManager downloadManager;
    QString filename;
    QNetworkReply *downloadFileReply;
    QSqlDatabase rojaDatabase; 

    void convert();
    void unZipCHM();
    void decompressCHM();
    void cleanSource();
    void cleanAfterConvert();
    void convertDir(QDir dir);

    void createDatabase();
    void createTables();
    void setDefaultSettings();

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
