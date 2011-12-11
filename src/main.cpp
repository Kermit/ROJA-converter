#include <QtCore/QCoreApplication>
#include <QtCore/QTextCodec>

#include <iostream>

#include "config.h"
#include "chmfile.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );
    QTextCodec::setCodecForTr( QTextCodec::codecForName( "UTF-8" ) );

    cout << "Witaj w konwerterze RoJa. Program ten zaraz pobierze plik CHM z sieci lub Twojego dysku twardego";
    cout << " i przekonwertuje do bazy danych SQLite." << endl;
    cout << endl;

    Config::checkConfig();

    CHMFile chmFile;
    chmFile.startConvert();

    return a.exec();
}
