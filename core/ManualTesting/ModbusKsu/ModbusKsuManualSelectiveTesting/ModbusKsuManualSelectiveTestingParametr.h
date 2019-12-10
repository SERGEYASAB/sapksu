#ifndef MODBUSKSUMANUALSELECTIVETESTINGPARAMETR_H
#define MODBUSKSUMANUALSELECTIVETESTINGPARAMETR_H

#include "Common/PrefixHeader.h"

class ModbusVariable;

struct ModbusKsuManualSelectiveTestingParametr
{
    bool enabled = false;
    int address = -1;
    ModbusVariable *variable = nullptr;
    QVariant writedValue;
    QVariant readedValue;
};

typedef QList<ModbusKsuManualSelectiveTestingParametr *> ModbusKsuManualSelectiveTestingParametrsList;

#endif // MODBUSKSUMANUALSELECTIVETESTINGPARAMETR_H
