#include "ModbusDeviceVariableValue.h"

template<>
quint8 ModbusDeviceVariableValue<bool>::bytesCount() const
{
    return 2;
}

template<>
quint8 ModbusDeviceVariableValue<QDateTime>::bytesCount() const
{
    return 6;
}
