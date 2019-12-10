#ifndef ENERGOFORMWORDVARIABLE_H
#define ENERGOFORMWORDVARIABLE_H

#include "Common/PrefixHeader.h"
#include "EnergoFormVariable.h"

template <typename T>
class ModbusDeviceVariableValue;

class EnergoFormWordVariable : public EnergoFormVariable
{
public:
    explicit EnergoFormWordVariable(QObject *parent = nullptr);

protected:
    IDeviceVariableValue *deviceVariableValue() const override;

private:
    ModbusDeviceVariableValue<qint16> *_variableValue = nullptr;
};

#endif // ENERGOFORMWORDVARIABLE_H
