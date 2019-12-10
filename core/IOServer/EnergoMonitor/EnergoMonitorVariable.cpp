#include "EnergoMonitorVariable.h"

EnergoMonitorVariable::EnergoMonitorVariable(IDeviceVariableValue *variableValue, QObject *parent) :
    IVariable(parent),
    m_variableValue(variableValue)
{

}

QVariant EnergoMonitorVariable::readValue()
{
    return value();
}

IDeviceVariableValue *EnergoMonitorVariable::deviceVariableValue() const
{
    return m_variableValue;
}

QByteArray EnergoMonitorVariable::convertByteOrder(const QByteArray &bytes) const
{
    QByteArray result;

    if (bytes.size() % 2 != 0 || bytes.isEmpty())
        return result;

    result.reserve(bytes.size());

    int size = bytes.size();
    for (int i = 0; i < size; i = i + 2) {
        result[i] = bytes[i + 1];
        result[i + 1] = bytes[i];
    }
    return result;
}
