#include "chmfile.h"
#include "config.h"

#include <QtCore/QCoreApplication>
#include <QUrl>
#include <QNetworkReply>
#include <QtCore/QRegExp>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QProcess>

#include <iostream>

using namespace std;

CHMFile::CHMFile(QObject *parent) :
    QObject(parent)
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(DownloadFinished(QNetworkReply*)));
}

void CHMFile::convertCHM()
{
    QUrl url("http://rozklady.kzkgop.pl/index.php?co=rozklady&submenu=pobierz");
    QNetworkRequest request(url);
    manager.get(request);
}

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

        if (!QFile(Config::homeDir() + "/" + filename + ".zip").exists())
        {
            QNetworkRequest downloadFileRequest(QUrl("http://rozklady.kzkgop.pl/pobierz/" + filename + ".zip"));
            downloadFileReply = manager.get(downloadFileRequest);
            connect(downloadFileReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(DownloadProgress(qint64,qint64)));
        }
        else
        {
            unZipCHM();
            decompressCHM();
        }
    }
    else
    {
        cout << endl;

        QFile file(Config::homeDir() + "/" + filename + ".zip");
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(reply->readAll());
            file.close();
        }

        unZipCHM();
        decompressCHM();
    }

}

void CHMFile::DownloadProgress(qint64 downloadedLenght, qint64 totalLenght)
{
    int percent = (downloadedLenght * 100)/totalLenght;
    QString text = QString("Pobrano %1% (%2/%3)").
            arg(QString::number(percent), QString::number(downloadedLenght), QString::number(totalLenght));

    cout << text.toStdString() << '\xd';
}

void CHMFile::unZipCHM()
{
    QString program("unzip");
    QStringList arguments;
    arguments << "-o";
    arguments << "-d" + Config::homeDir();
    arguments << Config::homeDir() + "/" + filename + ".zip";

    QProcess unzip(this);
    unzip.execute(program, arguments);
}

void CHMFile::decompressCHM()
{
    QString program("extract_chmLib");
    QStringList arguments;
    arguments << Config::homeDir() + "/" + filename + ".chm" << Config::homeDir() + "/" + "Temp";

    QProcess decompress(this);
    decompress.execute(program, arguments);
}
