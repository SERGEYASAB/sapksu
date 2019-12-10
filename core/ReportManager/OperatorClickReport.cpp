#include "OperatorClickReport.h"
#include "Authentication/SystemUser.h"
#include "DBConfig/DBConfig.h"

OperatorClickReport *OperatorClickReport::instance()
{
    static OperatorClickReport singletone;
    return &singletone;
}

OperatorClickReport::OperatorClickReport()
{

}

void OperatorClickReport::write(const QString &message)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("INSERT INTO `OperatorClickReport` "
                                "(user, message)"
                                "VALUES('%1', '%2');")
            .arg(SystemUser::instance()->user())

            .arg(message);

    QSqlQuery query(db);
    db.transaction();
    query.exec(sqlString);
    db.commit();
}

OperatorClickReport::~OperatorClickReport()
{

}
