#ifndef WORKMODEDIALOGMAPPER_H
#define WORKMODEDIALOGMAPPER_H

#include "Common/PrefixHeader.h"
#include "DatabaseDirectory/InputDialogMapperInterface.h"

class WorkModeDialogMapper : public InputDialogMapperInterface
{
public:
    explicit WorkModeDialogMapper(int comProtocolCode, QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);

    void save() override;

private:
    int m_comProtocolCode;
};

#endif // WORKMODEDIALOGMAPPER_H
