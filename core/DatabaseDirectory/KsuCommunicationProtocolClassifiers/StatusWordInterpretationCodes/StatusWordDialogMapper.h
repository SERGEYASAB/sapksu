#ifndef STATUSWORDDIALOGMAPPER_H
#define STATUSWORDDIALOGMAPPER_H

#include "Common/PrefixHeader.h"
#include "DatabaseDirectory/InputDialogMapperInterface.h"

class StatusWordDialogMapper : public InputDialogMapperInterface
{
public:
    explicit StatusWordDialogMapper(int comProtocolCode, QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);

    void save() override;

private:
    int m_comProtocolCode;
};

#endif // STATUSWORDDIALOGMAPPER_H
