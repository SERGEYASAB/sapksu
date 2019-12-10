#ifndef MODBUSKSUMANUALTESTINGPARAMETR_H
#define MODBUSKSUMANUALTESTINGPARAMETR_H

#include "Common/CommonEnums.h"

class ModbusVariable;

struct ModbusKsuManualTestingParametr
{
    ModbusKsuManualTestingParametr(ModbusVariable *variable) noexcept : m_variable(variable) {}

    inline ModbusVariable *variable() const noexcept { return m_variable; }

    bool enabled = true;
    QVariant writingValue;
    QVariant oldReadedValue;
    QVariant writedValue;
    QVariant readedValue;
    QString statusString;
    SapKsu::FullTestingStatus status = SapKsu::FullTestingStatus::Unknown;

    inline void clear();
    inline void clearWritingValue();

private:
    ModbusVariable *m_variable;
};

void ModbusKsuManualTestingParametr::clear()
{
    oldReadedValue.clear();
    writedValue.clear();
    readedValue.clear();
    statusString.clear();
    status = SapKsu::FullTestingStatus::Unknown;
}

void ModbusKsuManualTestingParametr::clearWritingValue()
{
    writingValue.clear();
}

#endif // MODBUSKSUMANUALTESTINGPARAMETR_H
