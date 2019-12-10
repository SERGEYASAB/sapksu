#include "EnergoFormVariable.h"

#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/IDeviceVariableValue.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/NumberVariableValueStrategy.h"

EnergoFormVariable::EnergoFormVariable(QObject *parent) : IVariable(parent)
{
    setValueStrategyType(IVariable::NumberStrategy);
    setServerValueDataType(IVariable::Float32);
}

QByteArray EnergoFormVariable::convertByteOrder(const QByteArray &bytes) const
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

double EnergoFormVariable::multiplier() const
{
    NumberVariableValueStrategy *strategy = dynamic_cast<NumberVariableValueStrategy *>(valueStrategy());
    if (!strategy)
        return 0;

    double coefficientK = strategy->coefficientK();
    double multiplier = 1.0 / coefficientK;
    return multiplier;
}

void EnergoFormVariable::setMultiplier(double multiplier)
{
    NumberVariableValueStrategy *strategy = dynamic_cast<NumberVariableValueStrategy *>(valueStrategy());
    if (!strategy)
        return;

    multiplier = 1 / multiplier;

    if (multiplier != strategy->coefficientK()) {
        strategy->setCoefficientK(multiplier);
        setDeviceVariantValue(value());
    }
}

QVariant EnergoFormVariable::readValue()
{
    return value();
}

void EnergoFormVariable::writeValue(const QVariant &value)
{
    setValue(value);
}

void EnergoFormVariable::setValue(const QVariant &value)
{
    if (value.isNull())
        return;

    if (!minValue().isNull() && value < minValue())
        return;

    if (!maxValue().isNull() && value > maxValue())
        return;

    if (!valueStrategy())
        return;

    QVariant deviceValue = deviceVariableValue()->deviceValueFromValueWithStrategy(value, *valueStrategy());
    setDeviceVariantValue(deviceValue);
}

QByteArray EnergoFormVariable::valueBytes() const
{
    if (!deviceVariableValue())
        return QByteArray();

    return convertByteOrder(deviceVariableValue()->currentValueBytes());
}
