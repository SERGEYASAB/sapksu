#ifndef MODBUSKSUMANUALTESTINGEVENTSARCHIVEWIDGET_H
#define MODBUSKSUMANUALTESTINGEVENTSARCHIVEWIDGET_H

#include "Common/PrefixHeader.h"
#include "ModbusKsuManualTestingArchivesForm.h"

class ModbusKsuNode;
class ModbusKsuManualTestingEventsArchiveModel;

class ModbusKsuManualTestingEventsArchiveWidget : public ModbusKsuManualTestingArchivesForm
{
    Q_OBJECT

public:
    explicit ModbusKsuManualTestingEventsArchiveWidget(ModbusKsuNode *node, QWidget *parent = nullptr);

    // ModbusKsuManualTestingArchivesForm interface
protected:
    void clear() override;
    void readArchives(int count = -1, int offset = 0) override;

private:
    ModbusKsuNode *m_node;

    ModbusKsuManualTestingEventsArchiveModel *m_model;
};

#endif // MODBUSKSUMANUALTESTINGEVENTSARCHIVEWIDGET_H
