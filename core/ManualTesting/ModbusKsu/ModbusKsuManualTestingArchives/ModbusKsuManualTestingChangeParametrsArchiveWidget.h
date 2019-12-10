#ifndef MODBUSKSUMANUALTESTINGCHANGEPARAMETRSARCHIVEWIDGET_H
#define MODBUSKSUMANUALTESTINGCHANGEPARAMETRSARCHIVEWIDGET_H

#include "Common/PrefixHeader.h"
#include "ModbusKsuManualTestingArchivesForm.h"

class ModbusKsuNode;
class ModbusKsuManualTestingChangeParametrsArchiveModel;

class ModbusKsuManualTestingChangeParametrsArchiveWidget : public ModbusKsuManualTestingArchivesForm
{
    Q_OBJECT

public:
    explicit ModbusKsuManualTestingChangeParametrsArchiveWidget(ModbusKsuNode *node, QWidget *parent = nullptr);

    // ModbusKsuManualTestingArchivesForm interface
protected:
    void clear() override;
    void readArchives(int count = -1, int offset = 0) override;

private:
    ModbusKsuNode *m_node = nullptr;

    ModbusKsuManualTestingChangeParametrsArchiveModel *m_model;
};

#endif // MODBUSKSUMANUALTESTINGCHANGEPARAMETRSARCHIVEWIDGET_H
