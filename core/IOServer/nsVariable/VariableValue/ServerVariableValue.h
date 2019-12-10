#ifndef SERVERVARIABLEVALUE_H
#define SERVERVARIABLEVALUE_H

#include "Common/PrefixHeader.h"

class IServerVariableValue
{
public:
    virtual ~IServerVariableValue() noexcept = default;
    virtual QVariant value() const = 0;
    virtual void setValue(const QVariant &value) = 0;
};

template <typename T>
class ServerVariableValue : public IServerVariableValue
{
public:
    ~ServerVariableValue() noexcept;

    QVariant value() const override {
        return _value;
    }

    void setValue(const QVariant &value) override {
        if (value.canConvert<T>() && (value.type() != QVariant::String)) {
            _typedValue = value.value<T>();
            _value.setValue(_typedValue);
        } else {
            _value = value;
        }
    }

private:
    QVariant _value = QVariant();
    T _typedValue;
};

template <typename T>
ServerVariableValue<T>::~ServerVariableValue() noexcept = default;


#endif // SERVERVARIABLEVALUE_H
