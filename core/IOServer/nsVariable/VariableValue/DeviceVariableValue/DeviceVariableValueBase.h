#ifndef DEVICEVARIABLEVALUEBASE_H
#define DEVICEVARIABLEVALUEBASE_H

#include "Common/PrefixHeader.h"
#include "IDeviceVariableValue.h"

#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/EnumVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/NumberVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/BitsetVariableValueStrategy.h"

template <typename T>
class DeviceVariableValueBase : public IDeviceVariableValue
{
public:
    DeviceVariableValueBase(IVariable &variable) : IDeviceVariableValue(variable) {}

    ~DeviceVariableValueBase() {
        delete _deviceValue;
    }

    QVariant variantValue() const override {
        if (_deviceValue)
            return QVariant::fromValue(*_deviceValue);

        return QVariant();
    }

    quint8 bytesCount() const override {
        return sizeof(T);
    }
    quint8 registersCount() const override {
        return bytesCount() / 2 + bytesCount() % 2;
    }

    QVariant valueWithStrategy(const VariableValueStrategy &strategy) const override;
    QVariant deviceValueFromValueWithStrategy(const QVariant &variantValue, const VariableValueStrategy &strategy) override;
    QVariant valueFromDeviceValueWithStrategy(const QVariant &deviceValue, const VariableValueStrategy &strategy) override;

    void clearValue() override;
    bool rawValueIsEmpty() const override;

protected:
    typedef T OriginalType;

    void setValue(T value);
    void setDeviceVariantValue(const QVariant &variantValue) override;
    inline T *currentValue() const { return _deviceValue; }

private:
    QVariant valueWithStrategy(T *value, const VariableValueStrategy &strategy) const;
    T *_deviceValue = nullptr;
};

template <typename T>
QVariant DeviceVariableValueBase<T>::valueWithStrategy(const VariableValueStrategy &strategy) const
{
    return valueWithStrategy(_deviceValue, strategy);
}

template <typename T>
QVariant DeviceVariableValueBase<T>::deviceValueFromValueWithStrategy(const QVariant &variantValue,
                                                                      const VariableValueStrategy &valueStrategy)
{
    if (variantValue.isNull())
        return QVariant();

    const std::type_info &typeinfo = typeid(valueStrategy);
    T writedDeviceValue = T();
    bool ok = false;

    QVariant minValue = variable().minValue();
    QVariant maxValue = variable().maxValue();

    if (typeinfo.hash_code() == typeid(VariableValueStrategy).hash_code()) {

        writedDeviceValue = valueStrategy.deviceValueFromVariantValue(variantValue, minValue, maxValue, _deviceValue, &ok);
    } else if (typeinfo == typeid(NumberVariableValueStrategy)) {
        const NumberVariableValueStrategy &strategy = dynamic_cast<const NumberVariableValueStrategy &>(valueStrategy);
        writedDeviceValue = strategy.deviceValueFromVariantValue(variantValue, minValue, maxValue, _deviceValue, &ok);
    } else if (typeinfo == typeid(RegisterVariableValueStrategy)) {
        const RegisterVariableValueStrategy &strategy = dynamic_cast<const RegisterVariableValueStrategy &>(valueStrategy);
        writedDeviceValue = strategy.deviceValueFromVariantValue(variantValue, minValue, maxValue, _deviceValue, &ok);
    } else if (typeinfo == typeid(EnumVariableValueStrategy)) {
        const EnumVariableValueStrategy &strategy = dynamic_cast<const EnumVariableValueStrategy &>(valueStrategy);
        writedDeviceValue = strategy.deviceValueFromVariantValue(variantValue, minValue, maxValue, _deviceValue, &ok);
    } else if (typeinfo == typeid(BitsetVariableValueStrategy)) {
        const BitsetVariableValueStrategy &strategy = dynamic_cast<const BitsetVariableValueStrategy &>(valueStrategy);
        writedDeviceValue = strategy.deviceValueFromVariantValue(variantValue, minValue, maxValue, _deviceValue, &ok);
    }

    QVariant result = QVariant();
    if (ok) {
        result.setValue(writedDeviceValue);
    }

    return result;
}

template <typename T>
QVariant DeviceVariableValueBase<T>::valueFromDeviceValueWithStrategy(const QVariant &deviceValue, const VariableValueStrategy &strategy)
{
    if (deviceValue.isValid() && deviceValue.canConvert<T>()) {
        T value = deviceValue.value<T>();
        return valueWithStrategy(&value, strategy);
    }

    return QVariant();
}

template <typename T>
void DeviceVariableValueBase<T>::setValue(T value) {
    static_assert(!std::is_pointer<T>::value && !std::is_reference<T>::value, "T must not be pointer or reference");

    bool valueChanged = !(_deviceValue && *_deviceValue == value);

    delete _deviceValue;
    _deviceValue = new T(value);

    if (valueChanged || isForceUpdateValue()) {
        this->valueChanged();

        clearForceUpdateValue();
    }
}

template <typename T>
void DeviceVariableValueBase<T>::setDeviceVariantValue(const QVariant &variantValue)
{
    if (variantValue.canConvert<T>())
        this->setValue(variantValue.value<T>());
}

template <typename T>
QVariant DeviceVariableValueBase<T>::valueWithStrategy(T *value, const VariableValueStrategy &strategy) const
{
    const std::type_info &typeinfo = typeid(strategy);
    if (!value) {
        return QVariant();
    }

    QVariant minValue = variable().minValue();
    QVariant maxValue = variable().maxValue();

    T deviceValue = *value;
    QVariant valueFromDevice = QVariant();

    if (typeinfo == typeid(VariableValueStrategy)) {
        valueFromDevice = strategy.variantValueFromDevice(deviceValue, minValue, maxValue);
        //        valueFromDevice.setValue(deviceValue);
    } else if (typeinfo == typeid(NumberVariableValueStrategy)) {
        const NumberVariableValueStrategy &numberStrategy = dynamic_cast<const NumberVariableValueStrategy &>(strategy);
        valueFromDevice = numberStrategy.variantValueFromDevice(deviceValue, minValue, maxValue);
    } else if (typeinfo == typeid(RegisterVariableValueStrategy)) {
        const RegisterVariableValueStrategy &registerStrategy = dynamic_cast<const RegisterVariableValueStrategy &>(strategy);
        valueFromDevice = registerStrategy.variantValueFromDevice(deviceValue, minValue, maxValue);
    } else if (typeinfo == typeid(EnumVariableValueStrategy)) {
        const EnumVariableValueStrategy &enumStrategy = dynamic_cast<const EnumVariableValueStrategy &>(strategy);
        valueFromDevice = enumStrategy.variantValueFromDevice(deviceValue, minValue, maxValue);
    } else if (typeinfo == typeid(BitsetVariableValueStrategy)) {
        const BitsetVariableValueStrategy &bitsetStrategy = dynamic_cast<const BitsetVariableValueStrategy &>(strategy);
        valueFromDevice = bitsetStrategy.variantValueFromDevice(deviceValue, minValue, maxValue);
    }

    return valueFromDevice;
}

template<typename T>
void DeviceVariableValueBase<T>::clearValue()
{
    if (_deviceValue) {
        delete _deviceValue;
        _deviceValue = nullptr;
    }
}

template<typename T>
bool DeviceVariableValueBase<T>::rawValueIsEmpty() const
{
    if (_deviceValue)
        return false;
    return true;
}


template <>
QVariant DeviceVariableValueBase<QDateTime>::valueWithStrategy(QDateTime *value, const VariableValueStrategy &strategy) const;

template <>
QVariant DeviceVariableValueBase<QString>::valueWithStrategy(QString *value, const VariableValueStrategy &strategy) const;

template <>
QVariant DeviceVariableValueBase<QDateTime>::deviceValueFromValueWithStrategy(const QVariant &variantValue,
                                                                              const VariableValueStrategy &valueStrategy);
template <>
QVariant DeviceVariableValueBase<QString>::deviceValueFromValueWithStrategy(const QVariant &variantValue,
                                                                            const VariableValueStrategy &valueStrategy);

#endif // DEVICEVARIABLEVALUEBASE_H
