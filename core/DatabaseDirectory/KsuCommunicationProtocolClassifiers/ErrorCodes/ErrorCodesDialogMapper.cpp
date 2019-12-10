#include "ErrorCodesDialogMapper.h"
#include "ReportManager/FaultReport.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"

ErrorCodesDialogMapper::ErrorCodesDialogMapper(int comProtocolCode, QSqlQueryModel *model, QWidget *parent)
    : InputDialogMapperInterface(model, parent)
{
    m_comProtocolCode = comProtocolCode;
}

void ErrorCodesDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    if (m_mapper->currentIndex() == -1) {

        QString sqlString = QString("INSERT INTO `KSU753ErrorCodes` (`comProtocolCode`, `code`, `name`)"
                                    "VALUES (%1, %2, '%3');")
                            .arg(m_comProtocolCode)
                            .arg(m_codeLineEdit->text())
                            .arg(m_descriptionLineEdit->text());

        QSqlQuery query(db);

        if (query.exec(sqlString) == false)
        {
            ErrorClassifier::instance()->showError(10);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(10) + query.lastError().databaseText());
        } else {
            emit dataChanged();
            close();
        }
    } else {
        QString sqlString = QString("UPDATE `KSU753ErrorCodes`"
                                    "SET code = %1, name = '%2' "
                                    "WHERE code = %3 AND comProtocolCode = %4;")
                            .arg(m_codeLineEdit->text())
                            .arg(m_descriptionLineEdit->text())
                            .arg(m_model->record(m_mapper->currentIndex()).value("code").toString())
                            .arg(m_comProtocolCode);

        QSqlQuery query(db);

        if (query.exec(sqlString) == false)
        {
            ErrorClassifier::instance()->showError(10);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(10) + query.lastError().databaseText());
        } else {
            emit dataChanged();
            close();
        }
    }
}
