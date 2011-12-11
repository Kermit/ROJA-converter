#ifndef DATABASE_H
#define DATABASE_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtSql/QSqlDatabase>

#include "src/model/communes.h"
#include "src/model/stops.h"
#include "src/model/lines.h"
#include "src/model/routesdetails.h"

class Database : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase rojaDatabase;
    void createSettingsTable();
    void createCommunesTable();
    void createStopsTable();
    void createLinesTable();
    void createRouteDetailsTable();

public:
    explicit Database(QObject *parent = 0);
    void createDatabase();
    void setDefaultSettings(const QString &filename);
    void addToCommunesTable(QHash<int, Communes> &data);
    void addToStopsTable(QHash<QString, Stops> &data);
    void addToLinesTable(QHash<QString, Lines> &data);
    void addToRoutesDetailsTable(QHash<int, RoutesDetails> &data);
};

#endif // DATABASE_H
