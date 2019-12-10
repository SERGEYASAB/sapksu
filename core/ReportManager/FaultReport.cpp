#include "FaultReport.h"
#include "DBConfig/DBConfig.h"
#include "Authentication/SystemUser.h"

FaultReport *FaultReport::instance()
{
    static FaultReport singletone;
    return &singletone;
}

FaultReport::FaultReport()
{

}

void FaultReport::write(const QString &message)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("INSERT INTO `FaultReport` "
                                "(user, message)"
                                "VALUES('%1', '%2');")
            .arg(SystemUser::instance()->user())
            .arg(message);

    QSqlQuery query(db);
    db.transaction();
    query.exec(sqlString);
    db.commit();
}

FaultReport::~FaultReport()
{

}
