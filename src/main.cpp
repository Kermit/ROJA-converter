#include <QtCore/QCoreApplication>

#include <iostream>

#include "config.h"
#include "chmfile.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    cout << "Witaj w konwerterze RoJa. Program ten zaraz pobierze plik CHM z sieci lub Twojego dysku twardego";
    cout << " i przekonwertuje do bazy danych SQLite." << endl;
    cout << endl;

    Config::checkConfig();

    CHMFile chmFile;
    chmFile.startConvert();

    return a.exec();
}
