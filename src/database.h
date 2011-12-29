#ifndef DATABASE_H
#define DATABASE_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtSql/QSqlDatabase>

#include "src/model/communes.h"
#include "src/model/stops.h"
#include "src/model/lines.h"
#include "src/model/routes.h"
#include "src/model/routesdetails.h"
#include "src/model/times.h"
#include "src/model/days.h"

class Database : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase rojaDatabase;
    void createSettingsTable();
    void createCommunesTable();
    void createStopsTable();
    void createLinesTable();
    void createRouteTable();
    void createRouteDetailsTable();
    void createTimesTable();
    void createDaysTable();

public:
    explicit Database(QObject *parent = 0);
    bool openDatabase();
    void createDatabase();
    void setDefaultSettings(const QString &filename);
    void addToCommunesTable(QHash<int, Communes> &data);
    void addToStopsTable(QHash<QString, Stops> &data);
    void addToLinesTable(QHash<QString, Lines> &data);
    void addToRoutesTable(QHash<int, Routes> &data);
    void addToRoutesDetailsTable(QHash<int, RoutesDetails> &data);
    void addToTimesTable(QMultiHash<int, Times> &data);
    void addToDaysTable(QHash<QString, Days> &data);
    void closeDatabase();
    QString getDatabaseVersion();
};

#endif // DATABASE_H
