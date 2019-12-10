#ifndef ENERGOFORMDWORDVARIABLE_H
#define ENERGOFORMDWORDVARIABLE_H

#include "Common/PrefixHeader.h"
#include "EnergoFormVariable.h"

template <typename T>
class ModbusDeviceVariableValue;

class EnergoFormDWordVariable : public EnergoFormVariable
{
public:
    explicit EnergoFormDWordVariable(QObject *parent = nullptr);

protected:
    IDeviceVariableValue *deviceVariableValue() const override;

private:
    ModbusDeviceVariableValue<qint32> *_variableValue = nullptr;
};

#endif // ENERGOFORMDWORDVARIABLE_H
