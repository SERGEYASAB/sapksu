#ifndef DBCONFIG_H
#define DBCONFIG_H

#include <QObject>
#include "Common/PrefixHeader.h"

class DBConfig : public QObject
{
    Q_OBJECT
public:
    static DBConfig *instance();
    QSqlDatabase &configDatabase() { return _configDB; }

public:
    static QString dbFolderPath();
    static QString dbFilePath();
    static QString parametrsTemplatesFolderPath();
    static QString ksuParametrsFolderPath();
    static QString sapParametrsFolderPath();
    static QString systemAliasesPath();
    static QString ksuManual0x20TestingPrestsPath();
    static QString ksuManualSelectiveTestingPresetsPath();

signals:

public slots:

private:
    explicit DBConfig();
    virtual ~DBConfig();

    void setupDatabase();
    void setupSystemAliases(QSqlQuery &query);
    void setupKsuAliases(QSqlQuery &query);
    void setupSAPParametrs();

    QSqlDatabase _configDB;
};

#endif // DBCONFIG_H
