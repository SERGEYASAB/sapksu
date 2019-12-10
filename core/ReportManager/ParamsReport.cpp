#include "ParamsReport.h"
#include "Authentication/SystemUser.h"
#include "DBConfig/DBConfig.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "IOServer/IOServer.h"

ParamsReport *ParamsReport::instance()
{
    static ParamsReport singletone;
    return &singletone;
}

ParamsReport::ParamsReport(QObject *parent)
    : QObject(parent)
{

}

void ParamsReport::write(const ParamsReport::ParamsReportFunction &function, const QByteArray &tx, const QByteArray &rx,
                         const QString &aliasName,
                         const QString &aliasValue, const QString &result)
{
    QString txString = tx.toHex().toUpper();
    QString rxString = rx.toHex().toUpper();

    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("INSERT INTO `ParamsReport` "
                                "(user, ksuName, softwareVersion, typeSU, function, tx, rx, aliasName, aliasValue, result)"
                                "VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10');")
                        .arg(SystemUser::instance()->user())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                        .arg(IOServer::instance()->getTestHistoryReport()->typeSULineEdit())
                        .arg(function)
                        .arg(txString)
                        .arg(rxString)
                        .arg(aliasName)
                        .arg(aliasValue)
                        .arg(result);

    QSqlQuery query(db);
    db.transaction();
    query.exec(sqlString);
    db.commit();
}
