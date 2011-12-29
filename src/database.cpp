#include "database.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include "src/config.h"

#include <QStringList>

#include <iostream>

Database::Database(QObject *parent) :
    QObject(parent)
{
}

bool Database::openDatabase()
{
    bool result = false;
    rojaDatabase = QSqlDatabase::addDatabase("QSQLITE");
    rojaDatabase.setDatabaseName(Config::databasePath());
    result = rojaDatabase.open();

    if (!result)
    {
        std::cout << "Błąd otwierania bazy: " << rojaDatabase.lastError().text().toStdString() << std::endl;
    }

    return result;
}

void Database::createDatabase()
{
    createSettingsTable();
    createCommunesTable();
    createStopsTable();
    createLinesTable();
    createRouteTable();
    createRouteDetailsTable();
    createDaysTable();
    createTimesTable();
}

void Database::createSettingsTable()
{
    QSqlQuery settingsTable;
    settingsTable.exec("CREATE TABLE settings"
                       "(id integer primary key, "
                       "name varchar(20), "
                       "value varchar(10))");
}

void Database::setDefaultSettings(const QString &filename)
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

void Database::createCommunesTable()
{
    QSqlQuery communesTable;
    communesTable.exec("CREATE TABLE communes"
                       "(id integer PRIMARY KEY, "
                       "name varchar(20), "
                       "number integer)");
}

void Database::addToCommunesTable(QHash<int, Communes> &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO communes (id, name, number) VALUES (:id, :name, :number)");

    rojaDatabase.transaction();
    foreach(int key, data.keys())
    {
        Communes commune = data.value(key);
        query.bindValue(":id", commune.getID());
        query.bindValue(":name", commune.getName());
        query.bindValue(":number", commune.getNumber());
        std::cout << "Dodaję gminę: " << commune.toString().toStdString() << std::endl;
        query.exec();
    }
    rojaDatabase.commit();
}

void Database::createStopsTable()
{
    QSqlQuery stopsTable;
    stopsTable.exec("CREATE TABLE stops"
                       "(id integer PRIMARY KEY, "
                       "name varchar(50), "
                       "communeID integer, "
                       "foreign key(communeID) references communes(id))");
}

void Database::addToStopsTable(QHash<QString, Stops> &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO stops (id, name, communeID) VALUES (:id, :name, :communeID)");

    rojaDatabase.transaction();
    foreach(QString key, data.keys())
    {
        Stops stop = data.value(key);
        query.bindValue(":id", stop.getID());
        query.bindValue(":name", stop.getName());
        query.bindValue(":communeID", stop.getCommuneID());        
        std::cout << "Dodaję przystanek: " << stop.toString().toStdString() << std::endl;
        query.exec();
    }
    rojaDatabase.commit();
}

void Database::createLinesTable()
{
    QSqlQuery linesTable;
    linesTable.exec("CREATE TABLE lines"
                       "(id integer PRIMARY KEY, "
                       "number varchar(10), "
                       "route1ID integer, "
                       "route2ID integer, "
                       "foreign key(route1ID) references routes(id), "
                       "foreign key(route2ID) references routes(id))");
}

void Database::addToLinesTable(QHash<QString, Lines> &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO lines (id, number, route1ID, route2ID) VALUES (:id, :number, :route1ID, :route2ID)");

    rojaDatabase.transaction();
    foreach(QString key, data.keys())
    {
        Lines line = data.value(key);
        query.bindValue(":id", line.getID());
        query.bindValue(":number", line.getNumber());
        query.bindValue(":route1ID", line.getRoute1ID());
        query.bindValue(":route2ID", line.getRoute2ID());
        std::cout << "Dodaję linię: " << line.toString().toStdString() << std::endl;
        query.exec();
    }
    rojaDatabase.commit();
}

void Database::createRouteTable()
{
    QSqlQuery routesTable;
    routesTable.exec("CREATE TABLE routes"
                       "(id integer PRIMARY KEY, "
                       "startID integer, "
                       "stopID integer, "
                       "lineID integer, "
                       "foreign key(startID) references stops(id), "
                       "foreign key(stopID) references stops(id), "
                       "foreign key(lineID) references lines(id))");
}

void Database::addToRoutesTable(QHash<int, Routes> &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO routes (id, startID, stopID, lineID)"
                  " VALUES (:id, :startID, :stopID, :lineID)");

    rojaDatabase.transaction();
    foreach(int key, data.keys())
    {
        Routes routes = data.value(key);
        query.bindValue(":id", routes.getID());
        query.bindValue(":startID", routes.getStartID());
        query.bindValue(":stopID", routes.getStopID());
        query.bindValue(":lineID", routes.getLineID());
        std::cout << "Dodaję trasę: " << routes.toString().toStdString() << std::endl;
        query.exec();
    }
    rojaDatabase.commit();
}

void Database::createRouteDetailsTable()
{
    QSqlQuery routesDetailsTable;
    routesDetailsTable.exec("CREATE TABLE routesDetails"
                       "(id integer PRIMARY KEY, "
                       "routeID integer, "
                       "stopID integer, "
                       "lineID integer, "
                       "routeDetailsID integer, "
                       "onDemand integer, "
                       "obligatory integer, "
                       "foreign key(stopID) references stops(id), "
                       "foreign key(lineID) references lines(id), "
                       "foreign key(routeDetailsID) references routes(id))");
}

void Database::addToRoutesDetailsTable(QHash<int, RoutesDetails> &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO routesDetails (id, routeID, stopID, lineID, routeDetailsID, onDemand, obligatory)"
                  " VALUES (:id, :routeID, :stopID, :lineID, :routeDetailsID, :onDemand, :obligatory)");

    rojaDatabase.transaction();
    foreach(int key, data.keys())
    {
        RoutesDetails routesDetails = data.value(key);
        query.bindValue(":id", routesDetails.getID());
        query.bindValue(":routeID", routesDetails.getRouteID());
        query.bindValue(":stopID", routesDetails.getStopID());
        query.bindValue(":lineID", routesDetails.getLineID());
        query.bindValue(":routeDetailsID", routesDetails.getRoutesDetailsID());
        query.bindValue(":onDemand", routesDetails.getOnDemand());
        query.bindValue(":obligatory", routesDetails.getObligatory());
        std::cout << "Dodaję trasę: " << routesDetails.toString().toStdString() << std::endl;
        query.exec();
    }
    rojaDatabase.commit();
}

void Database::createTimesTable()
{
    QSqlQuery timesTable;
    timesTable.exec("CREATE TABLE times"
                       "(dayID integer, "
                       "routeDetailsID integer, "
                       "time text, "
                       "legend text)");

    std::cout << timesTable.lastError().text().toStdString() << "\n";
}

void Database::addToTimesTable(QMultiHash<int, Times> &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO times (dayID, routeDetailsID, time, legend)"
                  " VALUES (:dayID, :routeDetailsID, :time, :legend)");

    rojaDatabase.transaction();
    data.remove(-1);
    if (data.keys().contains(-1))
        int ss = 0;

    foreach(int key, data.uniqueKeys())
    {
        QList<Times> times = data.values(key);

        while(!times.isEmpty())
        {
            Times time = times.takeLast();
            query.bindValue(":dayID", time.getDayID());
            query.bindValue(":routeDetailsID", time.getRouteDetailsID());
            query.bindValue(":time", time.getTime());
            query.bindValue(":legend", time.getLegend());
            std::cout << "Dodaję godzinę: " << time.toString().toStdString() << "\n";
            query.exec();
        }
    }
    rojaDatabase.commit();
}

void Database::createDaysTable()
{
    QSqlQuery daysTable;
    daysTable.exec("CREATE TABLE days"
                       "(id integer, "
                       "number integer, "
                       "name varchar(20))");
}

void Database::addToDaysTable(QHash<QString, Days> &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO days (id, number, name) VALUES (:id, :number, :name)");

    rojaDatabase.transaction();
    foreach(QString key, data.keys())
    {
        Days day = data.value(key);
        query.bindValue(":id", day.getID());
        query.bindValue(":number", day.getNumber());
        query.bindValue(":name", day.getName());
        std::cout << "Dodaję dzień: " << day.toString().toStdString() << std::endl;
        query.exec();
    }
    rojaDatabase.commit();
}

void Database::closeDatabase()
{
    rojaDatabase.close();
}

QString Database::getDatabaseVersion()
{
    QString version = NULL;

    if (openDatabase())
    {
        QSqlTableModel settingsTable(this, rojaDatabase);
        settingsTable.setTable("settings");
        settingsTable.setFilter("name='version'");
        settingsTable.select();
        version = settingsTable.lastError().text();
        version = settingsTable.record(0).value(2).toString();

        closeDatabase();
    }

    return version;
}
