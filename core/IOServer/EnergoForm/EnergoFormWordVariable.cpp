#include "EnergoFormWordVariable.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/ModbusDeviceVariableValue.h"

EnergoFormWordVariable::EnergoFormWordVariable(QObject *parent) :
    EnergoFormVariable(parent),
    _variableValue(new ModbusDeviceVariableValue<qint16>(*this))
{
    setDeviceVariantValue(0);
}

IDeviceVariableValue *EnergoFormWordVariable::deviceVariableValue() const
{
    return _variableValue;
}
