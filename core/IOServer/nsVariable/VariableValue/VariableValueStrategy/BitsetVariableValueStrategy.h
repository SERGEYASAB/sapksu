#ifndef BITSETVARIABLEVALUESTRATEGY_H
#define BITSETVARIABLEVALUESTRATEGY_H

#include "Common/PrefixHeader.h"
#include "VariableValueStrategy.h"

class BitsetVariableValueStrategy : public VariableValueStrategy
{
public:
    BitsetVariableValueStrategy() = default;
    virtual ~BitsetVariableValueStrategy() noexcept;

    template <typename T>
    QVariant variantValueFromDevice(T deviceValue,
                                    const QVariant &minValue,
                                    const QVariant &maxValue,
                                    typename std::enable_if<std::is_integral<T>::value>::type * = nullptr) const {
        if (useMinMax() && minValue.isValid() && !minValue.isNull() && minValue.canConvert<T>()) {
            T min = minValue.value<T>();
            deviceValue = qMax(deviceValue, min);
        }

        if (useMinMax() && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<T>()) {
            T max = maxValue.value<T>();
            deviceValue = qMin(deviceValue, max);
        }

        QString result = QString::number(deviceValue);

        return QVariant(deviceValue);

//        QStringList bitsStringValues;
//        for (auto it = _bitsValues.constBegin(); it != _bitsValues.constEnd(); it++) {
//            if ((deviceValue & it.key()) == it.key()) {
//                bitsStringValues.append(it.value());
//            }
//        }

//        if (!bitsStringValues.isEmpty()) {
//            result.append(QLatin1Literal(" (") % bitsStringValues.join(" | ") % QLatin1Literal(")"));
//        }

//        return result;
    }

    template <typename T>
    QVariant variantValueFromDevice(T deviceValue,
                                    const QVariant &minValue,
                                    const QVariant &maxValue,
                                    typename std::enable_if<!std::is_integral<T>::value>::type * = nullptr) const {
        Q_UNUSED(minValue)
        Q_UNUSED(maxValue)
        return QVariant::fromValue(deviceValue);
    }

    template <typename T>
    T deviceValueFromVariantValue(const QVariant &value,
                                  const QVariant &minValue,
                                  const QVariant &maxValue,
                                  T *deviceValue,
                                  bool *ok = nullptr) const {
        Q_UNUSED(deviceValue);


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

    QVariant descriptionValue(const QVariant &value) {
        if (value.canConvert<quint64>()) {
            quint64 intValue = value.value<quint64>();

            QString result = value.toString();

            QStringList bitsStringValues;
            for (auto it = _bitsValues.constBegin(); it != _bitsValues.constEnd(); it++) {
                if ((intValue & it.key()) == it.key()) {
                    bitsStringValues.append(it.value());
                }
            }

            if (!bitsStringValues.isEmpty()) {
                result.append(QLatin1Literal(" (") % bitsStringValues.join(" | ") % QLatin1Literal(")"));
            }

            return result;
        }

        return value;
    }

    inline const QHash<quint64, QString> &bitsValues() const { return _bitsValues; }
    inline void setBitsValues(const QHash<quint64, QString> &bitsValues) { _bitsValues = bitsValues; }

private:
    QHash<quint64, QString> _bitsValues;
};

#endif // BITSETVARIABLEVALUESTRATEGY_H
