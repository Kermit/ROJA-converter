#ifndef CHMFILE_H
#define CHMFILE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtNetwork/QNetworkAccessManager>

class QNetworkReply;

class CHMFile : public QObject
{
    Q_OBJECT

private:
    QNetworkAccessManager manager;
    QString filename;
    QNetworkReply *downloadFileReply;
    void unZipCHM();
    void decompressCHM();

public:
    explicit CHMFile(QObject *parent = 0);
    void convertCHM();

public slots:
    void DownloadFinished(QNetworkReply *reply);
    void DownloadProgress(qint64 downloadedLenght, qint64 totalLenght);
};

#endif // CHMFILE_H
