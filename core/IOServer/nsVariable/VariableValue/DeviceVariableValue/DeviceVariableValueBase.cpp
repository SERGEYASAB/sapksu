#include "DeviceVariableValueBase.h"

template <>
QVariant DeviceVariableValueBase<QDateTime>::deviceValueFromValueWithStrategy(const QVariant &variantValue,
                                                                              const VariableValueStrategy &valueStrategy)
{
    Q_UNUSED(valueStrategy)
    return variantValue;
}

template <>
QVariant DeviceVariableValueBase<QString>::deviceValueFromValueWithStrategy(const QVariant &variantValue,
                                                                            const VariableValueStrategy &valueStrategy)
{
    Q_UNUSED(valueStrategy)
    return variantValue;
}


template <>
QVariant DeviceVariableValueBase<QDateTime>::valueWithStrategy(QDateTime *value, const VariableValueStrategy &strategy) const
{
    Q_UNUSED(strategy)
    if (!value)
        return QVariant();

    return QVariant::fromValue(*value);
}

template <>
QVariant DeviceVariableValueBase<QString>::valueWithStrategy(QString *value, const VariableValueStrategy &strategy) const
{
    Q_UNUSED(strategy)
    if (!value)
        return QVariant();

    return QVariant::fromValue(*value);
}
