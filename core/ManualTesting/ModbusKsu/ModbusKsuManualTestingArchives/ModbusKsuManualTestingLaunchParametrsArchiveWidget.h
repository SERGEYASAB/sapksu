#ifndef MODBUSKSUMANUALTESTINGLAUNCHPARAMETRSARCHIVEWIDGET_H
#define MODBUSKSUMANUALTESTINGLAUNCHPARAMETRSARCHIVEWIDGET_H

#include "Common/PrefixHeader.h"
#include "ModbusKsuManualTestingArchivesForm.h"

class ModbusKsuNode;
class ModbusKsuManualTestingLaunchParametrsArchiveModel;

class ModbusKsuManualTestingLaunchParametrsArchiveWidget : public ModbusKsuManualTestingArchivesForm
{
    Q_OBJECT

public:
    explicit ModbusKsuManualTestingLaunchParametrsArchiveWidget(ModbusKsuNode *node, QWidget *parent = nullptr);

    // ModbusKsuManualTestingArchivesForm interface
protected:
    void clear() override;
    void readArchives(int count = -1, int offset = 0) override;

private:
    ModbusKsuNode *m_node;

    ModbusKsuManualTestingLaunchParametrsArchiveModel *m_model;
};

#endif // MODBUSKSUMANUALTESTINGLAUNCHPARAMETRSARCHIVEWIDGET_H
