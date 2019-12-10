#ifndef MODBUSARCHIVESEVENTCLASSESDIALOGMAPPER_H
#define MODBUSARCHIVESEVENTCLASSESDIALOGMAPPER_H

#include "Common/PrefixHeader.h"
#include "DatabaseDirectory/InputDialogMapperInterface.h"

class ModbusArchivesEventClassesDialogMapper : public InputDialogMapperInterface
{
    Q_OBJECT

public:
    ModbusArchivesEventClassesDialogMapper(int comProtocolCode, QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);

    void save() override;

private:
    int m_comProtocolCode;
};

#endif // MODBUSARCHIVESEVENTCLASSESDIALOGMAPPER_H
