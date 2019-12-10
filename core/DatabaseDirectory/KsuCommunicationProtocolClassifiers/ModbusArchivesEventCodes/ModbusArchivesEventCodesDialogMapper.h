#ifndef MODBUSARCHIVESEVENTCODESDIALOGMAPPER_H
#define MODBUSARCHIVESEVENTCODESDIALOGMAPPER_H

#include "Common/PrefixHeader.h"
#include "DatabaseDirectory/InputDialogMapperInterface.h"

class ModbusArchivesEventCodesDialogMapper : public InputDialogMapperInterface
{
    Q_OBJECT

public:
    ModbusArchivesEventCodesDialogMapper(int comProtocolCode, QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);

    void save() override;

private:
    int m_comProtocolCode;
};

#endif // MODBUSARCHIVESEVENTCODESDIALOGMAPPER_H
