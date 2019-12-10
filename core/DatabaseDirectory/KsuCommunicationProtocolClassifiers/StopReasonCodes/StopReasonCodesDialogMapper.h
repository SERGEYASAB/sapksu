#ifndef STOPREASONCODESDIALOGMAPPER_H
#define STOPREASONCODESDIALOGMAPPER_H

#include "Common/PrefixHeader.h"
#include "DatabaseDirectory/InputDialogMapperInterface.h"

class StopReasonCodesDialogMapper : public InputDialogMapperInterface
{
public:
    explicit StopReasonCodesDialogMapper(int comProtocolCode, QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);
    void save() override;

private:
    int m_comProtocolCode;
};

#endif // STOPREASONCODESDIALOGMAPPER_H
