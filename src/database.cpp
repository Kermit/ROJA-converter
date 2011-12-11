#include "database.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include "src/config.h"

#include <iostream>

Database::Database(QObject *parent) :
    QObject(parent)
{
}

void Database::createDatabase()
{
    rojaDatabase = QSqlDatabase::addDatabase("QSQLITE");
    rojaDatabase.setDatabaseName(Config::databasePath());
    if (rojaDatabase.open())
    {
        createSettingsTable();
        createCommunesTable();
        createStopsTable();
        createLinesTable();
        createRouteDetailsTable();
    }
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
                       "(id integer, "
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
                       "(id integer, "
                       "name varchar(50), "
                       "communeID integer)");
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
                       "(id integer, "
                       "number varchar(10), "
                       "route1ID integer, "
                       "route2ID integer)");
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

void Database::createRouteDetailsTable()
{
    QSqlQuery routesDetailsTable;
    routesDetailsTable.exec("CREATE TABLE routes"
                       "(id integer, "
                       "stopID integer, "
                       "lineID integer, "
                       "routeDetailsID integer)");
}

void Database::addToRoutesDetailsTable(QHash<int, RoutesDetails> &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO routes (id, stopID, lineID, routeDetailsID) VALUES (:id, :stopID, :lineID, :routeDetailsID)");

    rojaDatabase.transaction();
    foreach(int key, data.keys())
    {
        RoutesDetails routesDetails = data.value(key);
        query.bindValue(":id", routesDetails.getID());
        query.bindValue(":stopID", routesDetails.getStopID());
        query.bindValue(":lineID", routesDetails.getLineID());
        query.bindValue(":routeDetailsID", 1);
        std::cout << "Dodaję trasę: " << routesDetails.toString().toStdString() << std::endl;
        query.exec();
    }
    rojaDatabase.commit();
}
