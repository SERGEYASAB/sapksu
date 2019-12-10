#include "IVariable.h"

#include "VariableValue/DeviceVariableValue/IDeviceVariableValue.h"
#include "VariableValue/VariableValueTypeTraits.h"

#include "VariableValue/VariableValueStrategy/NumberVariableValueStrategy.h"
#include "VariableValue/VariableValueStrategy/EnumVariableValueStrategy.h"
#include "VariableValue/VariableValueStrategy/BitsetVariableValueStrategy.h"

IVariable::IVariable(QObject *parent) : IVariable(nullptr, parent)
{
}

IVariable::IVariable(QDomElement *node, QObject *parent)
    : QObject(parent),
      _readable(false),
      _writable(false),
      _period(0),
      _serverValueDataType(UInt16),
      //      _serverVariableValue(new ServerVariableValue<typename VariableValueTypeTraits<Uint16>::type>()),
      _serverVariableValue(createServerVariableValue()),
      _valueStrategyType(NoneStrategy),
      _valueStrategy(createValueStrategy()),
      _configNode(node),
      m_lastError(NodeError::noError())
{
}

IVariable::~IVariable()
{
    delete _serverVariableValue;
    delete _valueStrategy;
}

//IVariableValue *IVariable::variableValue() const
//{
//    return variableValue();
//}


QVariant IVariable::value() const
{
    return _serverVariableValue->value();
}

QVariant IVariable::readValue()
{
    emit readValueSync(this);
    return _serverVariableValue->value();
}

QVariant IVariable::descriptionValue() const
{
    return descriptionValue(_serverVariableValue->value());
}

QVariant IVariable::valueFromDeviceVariantValue(const QVariant &deviceValue) const
{
    if (!deviceVariableValue())
        return QVariant();

    return deviceVariableValue()->valueFromDeviceValueWithStrategy(deviceValue, *_valueStrategy);
}

void IVariable::setDeviceVariantValue(const QVariant &value)
{
    if (!deviceVariableValue())
        return;

    deviceVariableValue()->setDeviceVariantValue(value);


    //    if (_value == value) {
    //        return;
    //    }

    //    _value.setValue(value);
    //    emit valueChanged(this);


    //    new code
    //    // master
    //    if (_deviceValue == value) {
    //        return;
    //    }
    //    _deviceValue = value;

    //    if (_dataType == Register) {
    //        if (!value.canConvert<quint16>()) {
    //            return;
    //        }

    //        _registerBits.deviceValue = value.value<quint16>();
    //        _value = QVariant(_registerBits.maskedDeviceValue());
    //    } else {
    //        _value = valueFromDeviceValue(_deviceValue);
    //    }

    //    --old code

    //    if (_dataType == Register && value.canConvert<quint16>()) {
    //        _registerBits.deviceValue = value.value<quint16>();
    //    } else if (_value == value) {
    //        return;
    //    }



    //    _deviceValue.setValue(value);

    //    _value.setValue(value);
    //    _value.swap(valueFromDeviceValue(value));

}

void IVariable::setValueBytes(const QByteArray &bytes)
{
    if (!deviceVariableValue())
        return;

    deviceVariableValue()->setValueBytes(convertByteOrder(bytes));
    //    QVariant newValue = variableValue()->valueWithStrategy(*_valueStrategy);
    //    if (_serverVariableValue->value() != newValue) {
    //        _serverVariableValue->setValue(newValue);
    //        emit valueChanged(this);
    //    }
}

void IVariable::recreateValueStrategy()
{
    delete _valueStrategy;
    _valueStrategy = createValueStrategy();
}

QByteArray IVariable::writeValueBytes(const QVariant &value)
{
    if (!deviceVariableValue())
        return QByteArray();

    if (deviceVariableValue()->rawValueIsEmpty() && (_valueStrategyType == RegisterStrategy || _valueStrategyType == EnumStrategy)) {
        emit readValueSync(this);
    }

    QVariant deviceValue = deviceVariableValue()->deviceValueFromValueWithStrategy(value, *_valueStrategy);
    QByteArray deviceValueBytes = deviceVariableValue()->bytesFromValue(deviceValue);

    if (deviceValueBytes.isEmpty()) {
        return QByteArray();
    }

    QByteArray bytes = convertByteOrder(deviceValueBytes);
    return bytes;
}

QVariant IVariable::descriptionValue(const QVariant &value) const
{
    EnumVariableValueStrategy *enumStrategy = dynamic_cast<EnumVariableValueStrategy *>(valueStrategy());
    BitsetVariableValueStrategy *bitsetStrategy = dynamic_cast<BitsetVariableValueStrategy *>(valueStrategy());
    if (enumStrategy && valueStrategyType() == IVariable::EnumStrategy) {
        return enumStrategy->descriptionValue(value);
    } else if (bitsetStrategy && valueStrategyType() == IVariable::BitsetStrategy) {
        return bitsetStrategy->descriptionValue(value);
    } else {
        QMetaType::Type type = static_cast<QMetaType::Type>(value.type());
        if (type == QMetaType::Double || type == QMetaType::Float) {
            return SapKsu::outputToPrecision(value.toReal(), precision());
//            return QString::number(value.toReal(), 'g', precision());
        } else if (type == QMetaType::Bool) {
            return value.toBool() ? "ON" : "OFF";
        }
    }

    return value;
}

void IVariable::clearValue()
{
    if (deviceVariableValue()) {
        deviceVariableValue()->clearValue();
    }

    _serverVariableValue->setValue(QVariant());
}

void IVariable::setLastError(const NodeError &e)
{
    if (m_lastError != e) {
        m_lastError = e;
        if (m_lastError != NodeError::noError()) {
            emit error();
        }
    }
}

IServerVariableValue *IVariable::createServerVariableValue() const
{
    switch (_serverValueDataType) {
        case Boolean:
            return new ServerVariableValue<bool>();
            break;

        case Int8:
            return new ServerVariableValue<qint8>();
            break;

        case UInt8:
            return new ServerVariableValue<quint8>();
            break;

        case Int16:
            return new ServerVariableValue<qint16>();
            break;

        case Int32:
            return new ServerVariableValue<qint32>();
            break;

        case UInt32:
            return new ServerVariableValue<quint32>();
            break;

        case Int64:
            return new ServerVariableValue<qint64>();
            break;

        case UInt64:
            return new ServerVariableValue<quint64>();
            break;

        case Float32:
            return new ServerVariableValue<float>();
            break;

        case Float64:
            return new ServerVariableValue<double>();
            break;

        case DateTime:
            return new ServerVariableValue<QDateTime>();
            break;

        case String:
            return new ServerVariableValue<QString>();
            break;

        case UInt16:
        default:
            return new ServerVariableValue<quint16>();
            break;
    }
}

VariableValueStrategy *IVariable::createValueStrategy() const
{
    switch (_valueStrategyType) {
        case NumberStrategy:
            return new NumberVariableValueStrategy();
            break;

        case RegisterStrategy:
            return new RegisterVariableValueStrategy(deviceVariableValue()->bytesCount());
            break;

        case EnumStrategy:
            return new EnumVariableValueStrategy(deviceVariableValue()->bytesCount());
            break;

        case BitsetStrategy:
            return new BitsetVariableValueStrategy();
            break;

        case NoneStrategy:
        default:
            return new VariableValueStrategy();
            break;
    }
}

void IVariable::deviceValueChanged()
{
    QVariant newValue = deviceVariableValue()->valueWithStrategy(*_valueStrategy);
    if (_serverVariableValue->value() != newValue) {
        _serverVariableValue->setValue(newValue);
        emit valueChanged(this);
    }

    //конец
    //    emit valueChanged(this);
}



void IVariable::setPrecision(int precision)
{
    if (precision < -1) {
        precision = -1;
    }

    if (_precision != precision) {
        _precision = precision;
        emit precisionChanged();
    }
}

void IVariable::setValueStrategyType(const StrategyType &valueStrategyType)
{
    if (_valueStrategyType == valueStrategyType && _valueStrategy)
        return;

    _valueStrategyType = valueStrategyType;
    delete _valueStrategy;
    _valueStrategy = nullptr;

    _valueStrategy = createValueStrategy();
    if (_valueStrategy) {
        valueStrategy()->setUseMinMax(_useMinMax);
    }

    forceUpdateValue();
}

QVariant IVariable::defaultValue() const
{
    return _defaultValue;
}

void IVariable::setUseMinMax(bool useMinMax)
{
    _useMinMax = useMinMax;
    if (_valueStrategy) {
        valueStrategy()->setUseMinMax(_useMinMax);
    }
}

void IVariable::setPermissibleVariation(const QString &permissibleVariation)
{
    _permissibleVariation = permissibleVariation;
}

bool IVariable::hasError() const
{
    return m_lastError != NodeError::noError();
}

NodeError IVariable::lastError() const
{
    return m_lastError;
}

void IVariable::forceUpdateValue()
{
    if (!deviceVariableValue())
        return;

    deviceVariableValue()->forceUpdateValue();
    deviceValueChanged();
}

void IVariable::setServerValueDataType(const DataType &serverValueDataType)
{
    if (_serverValueDataType == serverValueDataType)
        return;

    _serverValueDataType = serverValueDataType;
    delete _serverVariableValue;

    //    _serverVariableValue = new ServerVariableValue<VariableValueTypeTraits<serverValueDataType>::type>();
    _serverVariableValue = createServerVariableValue();
}

bool IVariable::serverValueDataTypeIsSigned() const
{
    return _serverValueDataType == IVariable::Boolean ||
            _serverValueDataType == IVariable::UInt8 ||
            _serverValueDataType == IVariable::UInt16 ||
            _serverValueDataType == IVariable::UInt32 ||
            _serverValueDataType == IVariable::UInt64;
}

//void IVariable::setValue(QVariant &&value)
//{
//    if ()

//    if (_dataType == Register && value.canConvert<quint16>()) {
//        _registerBits.deviceValue = value.value<quint16>();
//    } else if (_value == value) {
//        return;
//    }

//    _deviceValue.swap(value);

//    _value.swap(value);
//    _value.swap(valueFromDeviceValue(value));
//    emit valueChanged(this);
//}

void IVariable::writeValue(const QVariant &value)
{
    if (!deviceVariableValue())
        return;

    if (deviceVariableValue()->rawValueIsEmpty() && (_valueStrategyType == RegisterStrategy || _valueStrategyType == EnumStrategy)) {
        emit readValueSync(this);
    }

    QVariant deviceValue = deviceVariableValue()->deviceValueFromValueWithStrategy(value, *_valueStrategy);
    QByteArray deviceValueBytes = deviceVariableValue()->bytesFromValue(deviceValue);

    if (deviceValueBytes.isEmpty()) {
        return;
    }

    QByteArray bytes = convertByteOrder(deviceValueBytes);
    emit writeDeviceValueAsync(this, deviceValue, bytes);
}

void IVariable::setValue(const QVariant &value)
{
    if (!deviceVariableValue())
        return;

    if (deviceVariableValue()->rawValueIsEmpty() && (_valueStrategyType == RegisterStrategy || _valueStrategyType == EnumStrategy)) {
        emit readValueSync(this);
    }

    QVariant deviceValue = deviceVariableValue()->deviceValueFromValueWithStrategy(value, *_valueStrategy);
    QByteArray deviceValueBytes = deviceVariableValue()->bytesFromValue(deviceValue);

    if (deviceValueBytes.isEmpty()) {
        return;
    }

    QByteArray bytes = convertByteOrder(deviceValueBytes);
    emit writeDeviceValueSync(this, deviceValue, bytes);
}

IVariable::UniqueWriteDataPtr IVariable::writeValueDataForValue(const QVariant &value)
{
    if (!deviceVariableValue() || !_valueStrategy)
        return IVariable::UniqueWriteDataPtr();

    Q_ASSERT(_valueStrategy);

    if (deviceVariableValue()->rawValueIsEmpty() && (_valueStrategyType == RegisterStrategy || _valueStrategyType == EnumStrategy)) {
        emit readValueSync(this);
    }

    QVariant deviceValue = deviceVariableValue()->deviceValueFromValueWithStrategy(value, *_valueStrategy);
    QByteArray deviceValueBytes = deviceVariableValue()->bytesFromValue(deviceValue);

    if (deviceValueBytes.isEmpty())
        return IVariable::UniqueWriteDataPtr();

    QByteArray bytes = convertByteOrder(deviceValueBytes);

    return IVariable::UniqueWriteDataPtr(new IVariable::WriteData(value, deviceValue, bytes));
}

QDomElement *IVariable::getConfigNode() const
{
    return _configNode;
}

void IVariable::setConfigNode(QDomElement *element)
{
    _configNode = element;
}

QString IVariable::path() const
{
    return _alias;
}

quint32 IVariable::period() const
{
    return _period;
}

void IVariable::setPeriod(quint32 period)
{
    _period = period;
}

bool IVariable::isReadable() const
{
    return _readable;
}

void IVariable::setReadable(bool readable)
{
    _readable = readable;
}

bool IVariable::isWritable() const
{
    return _writable;
}

void IVariable::setWritable(bool writable)
{
    _writable = writable;
}

bool IVariable::isBoolean() const
{
    return _serverValueDataType == Boolean;

    //    if (!variableValue())
    //        return false;
    //    return variableValue()->isBoolean();
}

quint32 IVariable::dataSize() const
{
    if (deviceVariableValue()) {
        return deviceVariableValue()->bytesCount();
    }

    return 0;
}
