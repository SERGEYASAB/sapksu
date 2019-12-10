#include "EnergoFormDWordVariable.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/ModbusDeviceVariableValue.h"

EnergoFormDWordVariable::EnergoFormDWordVariable(QObject *parent) :
    EnergoFormVariable(parent),
    _variableValue(new ModbusDeviceVariableValue<qint32>(*this))
{
    setDeviceVariantValue(0);
}

IDeviceVariableValue *EnergoFormDWordVariable::deviceVariableValue() const
{
    return _variableValue;
}
