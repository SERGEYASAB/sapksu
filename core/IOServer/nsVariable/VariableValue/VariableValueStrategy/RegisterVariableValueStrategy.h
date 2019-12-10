#ifndef REGISTERVARIABLEVALUESTRATEGY_H
#define REGISTERVARIABLEVALUESTRATEGY_H

#include "Common/PrefixHeader.h"
#include "VariableValueStrategy.h"

class RegisterVariableValueStrategy : public VariableValueStrategy
{
public:
    RegisterVariableValueStrategy() :
        VariableValueStrategy(), _valueBitsCount(16), _startBit(0), _endBit(_valueBitsCount - 1) {}

    RegisterVariableValueStrategy(quint8 bytesCount) :
        VariableValueStrategy(), _valueBitsCount(bytesCount * 8), _startBit(0), _endBit(_valueBitsCount - 1) {}

    template <typename T>
    RegisterVariableValueStrategy(T value) :
        VariableValueStrategy(), _valueBitsCount(sizeof(T) * 8), _startBit(0), _endBit(_valueBitsCount - 1) {
        Q_UNUSED(value)
    }

    ~RegisterVariableValueStrategy() noexcept;

    bool isBoolean() const override {
        return significantBitsCount() == 1;
    }

    template <typename T>
    QVariant variantValueFromDevice(T deviceValue, const QVariant &minValue, const QVariant &maxValue) const {
        auto value = this->maskedValue(deviceValue, _startBit, significantBitsCount());

        if (useMinMax() && minValue.isValid() && !minValue.isNull() && minValue.canConvert<T>()) {
            T min = minValue.value<T>();
            value = qMax(value, min);
        }

        if (useMinMax() && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<T>()) {
            T max = maxValue.value<T>();
            value = qMin(value, max);
        }

        return QVariant(value);
    }

    template <typename T>
    T deviceValueFromVariantValue(const QVariant &value,
                                  const QVariant &minValue,
                                  const QVariant &maxValue,
                                  T *deviceValue,
                                  bool *ok = nullptr) const {

        if (!deviceValue) {
            if (ok) *ok = false;
            return T();
        }

        if (!value.canConvert<T>()) {
            if (ok) *ok = false;
            return *deviceValue;
        }
        T settableValue = value.value<T>();

        if (useMinMax() && minValue.isValid() && !minValue.isNull() && minValue.canConvert<T>()) {
            if (settableValue < minValue.value<T>() && ok) *ok = false;
        }
        if (useMinMax() && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<T>()) {
            if (settableValue > maxValue.value<T>() && ok) *ok = false;
        }

        T result = valueWithSignificantBits(*deviceValue, settableValue, _startBit, significantBitsCount());
        if (ok) *ok = true;
        return result;
    }

    void setBytesCount(quint8 bytesCount);
    void setBitRange(quint8 startBit, quint8 endBit);

    inline quint8 startBit() const { return _startBit; }
    void setStartBit(quint8 startBit);

    inline quint8 endBit() const { return _endBit; }
    void setEndBit(quint8 endBit);

    inline quint8 significantBitsCount() const { return _endBit - _startBit + 1; }
    inline quint8 bitsCount() const { return _valueBitsCount; }

protected:
    template <typename T>
    inline T maskedValue(T value, quint8 startBit, quint8 bitsCount) const {
        return maskedValue_impl(value,
                                startBit,
                                bitsCount,
                                std::integral_constant<bool, !std::is_same<bool, typename std::decay<T>::type>::value &&
                                std::is_integral<T>::value>());
    }

    template <typename T>
    inline T valueWithSignificantBits(T originalValue, T settableValue, quint8 startBit, quint8 bitsCount) const {
        return valueWithSignificantBits_impl(originalValue,
                                             settableValue,
                                             startBit,
                                             bitsCount,
                                             std::integral_constant<bool, !std::is_same<bool, typename std::decay<T>::type>::value && std::is_integral<T>::value>());
    }

private:
    template <typename T>
    T maskedValue_impl(T value, quint8 startBit, quint8 bitsCount, std::true_type) const {
        T resultValue = 0;
        for (quint8 i = 0; i < bitsCount; i++) {
            quint8 testedBit = i + startBit;
            quint16 temp = static_cast<quint16>((value >> testedBit) & 1) << i;
            resultValue |= temp;
        }

        return resultValue;
    }

    template <typename T>
    T maskedValue_impl(T value, quint8, quint8, std::false_type) const {
        return value;
    }

    template <typename T>
    T valueWithSignificantBits_impl(T originalValue, T settableValue, quint8 startBit, quint8 bitsCount, std::true_type) const {
        T resultValue = originalValue;
        if (bitsCount == 1) {
            quint8 testedBit = startBit;
            bool bit = static_cast<bool>(settableValue);
            if (bit) {
                resultValue |= 1 << testedBit;
            } else {
                resultValue &= ~(1 << testedBit);
            }
        } else {
            for (quint8 i = 0; i < bitsCount; i++) {
                quint8 testedBit = i + startBit;
                bool bit = (settableValue & ( 1 << i )) >> i;
                if (bit) {
                    resultValue |= 1 << testedBit;
                } else {
                    resultValue &= ~(1 << testedBit);
                }
            }
        }
        return resultValue;
    }

    template <typename T>
    T valueWithSignificantBits_impl(T originalValue, T, quint8, quint8, std::false_type) const {
        return originalValue;
    }

    quint8 _valueBitsCount;

    quint8 _startBit;
    quint8 _endBit;
};

#endif // REGISTERVARIABLEVALUESTRATEGY_H
