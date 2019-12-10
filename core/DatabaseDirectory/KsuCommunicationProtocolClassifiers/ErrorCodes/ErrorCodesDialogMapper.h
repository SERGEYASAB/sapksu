#ifndef ERRORCODESDIALOGMAPPER_H
#define ERRORCODESDIALOGMAPPER_H

#include "Common/PrefixHeader.h"
#include "DatabaseDirectory/InputDialogMapperInterface.h"

class ErrorCodesDialogMapper : public InputDialogMapperInterface
{
public:
    explicit ErrorCodesDialogMapper(int comProtocolCode, QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);

    void save() override;

private:
    int m_comProtocolCode;
};

#endif // ERRORCODESDIALOGMAPPER_H
