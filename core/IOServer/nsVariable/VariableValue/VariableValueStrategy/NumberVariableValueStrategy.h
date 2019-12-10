#ifndef NUMBERVARIABLEVALUESTRATEGY_H
#define NUMBERVARIABLEVALUESTRATEGY_H

#include "Common/PrefixHeader.h"
#include "VariableValueStrategy.h"

template<typename T, typename Enable = void>
struct double2T
{
    inline static T convert(double value) {
        return static_cast<T>(value);
    }
};

template<typename T>
struct double2T<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
    inline static T convert(double value) {
        return static_cast<T>(round(value));
    }
};


class NumberVariableValueStrategy : public VariableValueStrategy
{
public:
    NumberVariableValueStrategy();
    NumberVariableValueStrategy(double coefficientK, double coefficientB);

    inline double coefficientK() const;
    void setCoefficientK(double coefficientK);

    inline double coefficientB() const;
    void setCoefficientB(double coefficientB);

    template <typename T>
    QVariant variantValueFromDevice(T deviceValue, const QVariant &minValue, const QVariant &maxValue) const {
        auto val = deviceValue * _coefficientK + _coefficientB;

        typedef decltype(val) ValueType;

        if (useMinMax() && minValue.isValid() && !minValue.isNull() && minValue.canConvert<ValueType>()) {
            ValueType min = minValue.value<ValueType>();
            val = qMax(val, min);
        }

        if (useMinMax() && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<ValueType>()) {
            ValueType max = maxValue.value<ValueType>();
            val = qMin(val, max);
        }

        return QVariant(val);
    }

    template <typename T>
    T deviceValueFromVariantValue(const QVariant &value,
                                  const QVariant &minValue,
                                  const QVariant &maxValue,
                                  T *deviceValue,
                                  bool *ok = nullptr) const {
        Q_UNUSED(deviceValue)

        if (ok) *ok = true;

        if (!value.canConvert<double>()) {
            if (ok) *ok = false;
            return T();
        }

        double convertedVariant = value.value<double>();
        if (useMinMax() && minValue.isValid() && !minValue.isNull() && minValue.canConvert<double>()) {
            if (convertedVariant < minValue.value<double>() && ok) *ok = false;
        }
        if (useMinMax() && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<double>()) {
            if (convertedVariant > maxValue.value<double>() && ok) *ok = false;
        }

        double rawResult = (convertedVariant - _coefficientB) / _coefficientK;
        T result = double2T<T>::convert(rawResult);
        return result;
    }

private:
    double _coefficientK = 1.0;
    double _coefficientB = 0.0;
};

inline double NumberVariableValueStrategy::coefficientK() const { return _coefficientK; }
inline double NumberVariableValueStrategy::coefficientB() const { return _coefficientB; }

#endif // NUMBERVARIABLEVALUESTRATEGY_H
