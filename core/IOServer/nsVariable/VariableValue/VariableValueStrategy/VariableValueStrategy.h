#ifndef VARIABLEVALUESTRATEGY_H
#define VARIABLEVALUESTRATEGY_H

#include "Common/PrefixHeader.h"

class VariableValueStrategy
{
public:
    VariableValueStrategy() = default;
    virtual ~VariableValueStrategy() noexcept = default;
    virtual bool isBoolean() const {
        return false;
    }

    template <typename T>
    QVariant variantValueFromDevice(T deviceValue, const QVariant &minValue, const QVariant &maxValue) const {
        if (useMinMax() && minValue.isValid() && !minValue.isNull() && minValue.canConvert<T>()) {
            T min = minValue.value<T>();
            deviceValue = qMax(deviceValue, min);
        }

        if (useMinMax() && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<T>()) {
            T max = maxValue.value<T>();
            deviceValue = qMin(deviceValue, max);
        }

        return QVariant::fromValue(deviceValue);
    }

    template <typename T>
    T deviceValueFromVariantValue(const QVariant &value,
                                  const QVariant &minValue,
                                  const QVariant &maxValue,
                                  T *deviceValue,
                                  bool *ok = nullptr) const {
        Q_UNUSED(deviceValue)

        if (ok) *ok = true;
        if (!value.canConvert<T>()) {
            if (ok) *ok = false;
            return T();
        }

        T result = value.value<T>();

        if (useMinMax() && minValue.isValid() && !minValue.isNull() && minValue.canConvert<T>()) {
            if (result < minValue.value<T>() && ok) *ok = false;
        }
        if (useMinMax() && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<T>()) {
            if (result > maxValue.value<T>() && ok) *ok = false;
        }

        return result;
    }

    inline bool useMinMax() const noexcept { return _useMinMax; }
    inline void setUseMinMax(bool useMinMax) noexcept { _useMinMax = useMinMax; }

private:
    bool _useMinMax = true;
};

#endif // VARIABLEVALUESTRATEGY_H
