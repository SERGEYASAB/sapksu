#ifndef ENERGOMONITORVARIABLE_H
#define ENERGOMONITORVARIABLE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsVariable/IVariable.h"

class EnergoMonitorNode;

class EnergoMonitorVariable : public IVariable
{
public:
    EnergoMonitorVariable(IDeviceVariableValue *variableValue, QObject *parent = nullptr);

    // IVariable interface
public:
    QVariant readValue() override;

protected:
    IDeviceVariableValue *deviceVariableValue() const override;
    QByteArray convertByteOrder(const QByteArray &bytes) const override;

private:
    IDeviceVariableValue *m_variableValue = nullptr;

    friend class EnergoMonitorNode;
};

#endif // ENERGOMONITORVARIABLE_H
