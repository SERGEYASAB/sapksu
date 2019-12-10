#ifndef IDEVICEVARIABLEVALUE_H
#define IDEVICEVARIABLEVALUE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/VariableValueStrategy.h"

class IDeviceVariableValue
{
    Q_GADGET

public:
    IDeviceVariableValue(IVariable &variable) : _variable(variable) {}
    virtual ~IDeviceVariableValue() {}

    virtual bool setValueBytes(const QByteArray &bytes) = 0;
    virtual QVariant valueWithStrategy(const VariableValueStrategy &strategy) const = 0;

    virtual QByteArray bytesFromValue(const QVariant &variantValue) const = 0;
    virtual QVariant deviceValueFromValueWithStrategy(const QVariant &variantValue, const VariableValueStrategy &strategy) = 0;
    virtual QVariant valueFromDeviceValueWithStrategy(const QVariant &deviceValue, const VariableValueStrategy &strategy) = 0;
    virtual QByteArray currentValueBytes() const = 0;

    virtual quint8 bytesCount() const = 0;
    virtual quint8 registersCount() const = 0; //1 register = DWord

    virtual QVariant variantValue() const = 0;
    virtual void clearValue() = 0;

    virtual bool rawValueIsEmpty() const = 0;

protected:
    virtual void setDeviceVariantValue(const QVariant &variantValue) = 0;

    inline const IVariable &variable() const {
        return _variable;
    }

    inline void valueChanged() const {
        _variable.deviceValueChanged();
    }

    inline void forceUpdateValue() {
        _forceUpdateValue = true;
    }

    inline void clearForceUpdateValue() {
        _forceUpdateValue = false;
    }

    inline bool isForceUpdateValue() const {
        return _forceUpdateValue;
    }

private:
    friend class IVariable;
    IVariable &_variable;
    bool _forceUpdateValue = false;
};

#endif // IDEVICEVARIABLEVALUE_H
