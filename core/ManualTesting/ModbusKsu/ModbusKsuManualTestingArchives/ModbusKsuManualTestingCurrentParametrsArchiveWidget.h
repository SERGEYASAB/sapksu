#ifndef MODBUSKSUMANUALTESTINGCURRENTPARAMETRSARCHIVEWIDGET_H
#define MODBUSKSUMANUALTESTINGCURRENTPARAMETRSARCHIVEWIDGET_H

#include "Common/PrefixHeader.h"
#include "ModbusKsuManualTestingArchivesForm.h"

class ModbusKsuNode;
class ModbusKsuManualTestingCurrentParametrsArchiveModel;

class ModbusKsuManualTestingCurrentParametrsArchiveWidget : public ModbusKsuManualTestingArchivesForm
{
    Q_OBJECT

public:
    explicit ModbusKsuManualTestingCurrentParametrsArchiveWidget(ModbusKsuNode *node, QWidget *parent = nullptr);

    // ModbusKsuManualTestingArchivesForm interface
protected:
    void clear() override;
    void readArchives(int count = -1, int offset = 0) override;

private:
    ModbusKsuNode *m_node;

    ModbusKsuManualTestingCurrentParametrsArchiveModel *m_model;
};

#endif // MODBUSKSUMANUALTESTINGCURRENTPARAMETRSARCHIVEWIDGET_H
