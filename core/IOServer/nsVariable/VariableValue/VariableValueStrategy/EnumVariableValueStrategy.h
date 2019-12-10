#ifndef ENUMVARIABLEVALUESTRATEGY_H
#define ENUMVARIABLEVALUESTRATEGY_H

#include "Common/PrefixHeader.h"
#include "RegisterVariableValueStrategy.h"

class EnumVariableValueStrategy : public RegisterVariableValueStrategy
{
public:
    EnumVariableValueStrategy() = default;
    EnumVariableValueStrategy(quint8 bytesCount) : RegisterVariableValueStrategy(bytesCount) {}
    ~EnumVariableValueStrategy() noexcept;

    inline const QMap<quint64, QString> &enumValues() const {
        return _enumValues;
    }

    inline void setEnumValues(const QMap<quint64, QString> &enumValues) {
        _enumValues = enumValues;
    }

    inline void setEnumValues(QMap<quint64, QString> &&enumValues) {
        _enumValues = std::move(enumValues);
    }

    bool isBoolean() const override {
        return false;
    }

    template <typename T>
    QVariant variantValueFromDevice(T deviceValue, const QVariant &minValue, const QVariant &maxValue) const {
        Q_UNUSED(minValue)
        Q_UNUSED(maxValue)

        auto value = this->maskedValue(deviceValue, startBit(), significantBitsCount());
        quint64 checkedValue = static_cast<quint64>(value);
//        if (_enumValues.contains(checkedValue)) {
//            return QVariant(_enumValues[checkedValue]);
//        }

//        return QVariant(value);

        return QVariant(checkedValue);
    }

    template <typename T>
    T deviceValueFromVariantValue(const QVariant &value,
                                  const QVariant &minValue,
                                  const QVariant &maxValue,
                                  T *deviceValue,
                                  bool *ok = nullptr) const {
        Q_UNUSED(minValue)
        Q_UNUSED(maxValue)

        if (!deviceValue) {
            if (ok) *ok = false;
            return T();
        }

        if (value.canConvert<quint64>()) {
            quint64 intVal = value.value<quint64>();
            if (_enumValues.contains(intVal)) {
                T result = valueWithSignificantBits(*deviceValue, static_cast<T>(intVal), startBit(), significantBitsCount());
                if (ok) *ok = true;
                return result;
            }
        }
        if (ok) *ok = false;
        return *deviceValue;
    }

    QVariant descriptionValue(const QVariant &value) {
        if (value.canConvert<quint64>()) {
            quint64 checkedValue = value.value<quint64>();
            if (_enumValues.contains(checkedValue)) {
                return QVariant(QString("%1 (%2)").arg(value.toString()).arg(_enumValues[checkedValue]));
            }
        }

        return value;
    }

private:
    QMap<quint64, QString> _enumValues;
};

#endif // ENUMVARIABLEVALUESTRATEGY_H
