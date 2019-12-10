#include "ModbusVariable.h"

//#include "IVariableValue/IVariableValue.h"
#include "VariableValue/DeviceVariableValue/ModbusDeviceVariableValue.h"

ModbusVariable::ModbusVariable(QObject *parent)
    : IVariable(parent), _address(0), _deviceDataType(Int16), _byteOrder(LittleEndian)
{
    _variableValue = createVariableValue(_deviceDataType);
    connect(this, &IVariable::precisionChanged, this, &ModbusVariable::precisionChange);
}



quint16 ModbusVariable::address() const
{
    return _address;
}

void ModbusVariable::setAddress(quint16 address)
{
    _address = address;
}

quint8 ModbusVariable::registersCount() const
{
    if (deviceVariableValue()) {
        return deviceVariableValue()->registersCount();
    }

    return 0;
}

ModbusVariable::ByteOrder ModbusVariable::byteOrder() const
{
    return _byteOrder;
}

void ModbusVariable::setByteOrder(ByteOrder byteOrder)
{
    _byteOrder = byteOrder;
}

ModbusVariable::ModbusDataType ModbusVariable::deviceDataType() const
{
    return _deviceDataType;
}

void ModbusVariable::setDeviceDataType(ModbusDataType dataType)
{
    if (_deviceDataType == dataType)
        return;

    _deviceDataType = dataType;

    delete _variableValue;
    _variableValue = createVariableValue(_deviceDataType);

    RegisterVariableValueStrategy *registerStrategy = dynamic_cast<RegisterVariableValueStrategy *>(valueStrategy());
    if (registerStrategy && registerStrategy->bitsCount() != _variableValue->bytesCount() * 8) {
        recreateValueStrategy();
    }
}

QVariant ModbusVariable::defaultValue() const
{
    if (_deviceDataType == ModbusKsuDate) {
        return QDateTime::currentDateTime();
    }
    return IVariable::defaultValue();
}

IDeviceVariableValue *ModbusVariable::createVariableValue(ModbusVariable::ModbusDataType dataType)
{
    IDeviceVariableValue *result = nullptr;

    switch (dataType) {
        case Boolean:
            result = new ModbusDeviceVariableValue<bool>(*this);
            break;

        case Int8:
            result = new ModbusDeviceVariableValue<qint8>(*this);
            break;

        case UInt8:
            result = new ModbusDeviceVariableValue<quint8>(*this);
            break;

        case Int16:
            result = new ModbusDeviceVariableValue<qint16>(*this);
            break;

        case Int32:
            result = new ModbusDeviceVariableValue<qint32>(*this);
            break;

        case UInt32:
            result = new ModbusDeviceVariableValue<quint32>(*this);
            break;

        case Int64:
            result = new ModbusDeviceVariableValue<qint64>(*this);
            break;

        case UInt64:
            result = new ModbusDeviceVariableValue<quint64>(*this);
            break;

        case Float32:
            result = new ModbusDeviceVariableValue<float>(*this);
            break;

        case Float64:
            result = new ModbusDeviceVariableValue<double>(*this);
            break;

        case ModbusKsuDate:
            result = new ModbusDeviceVariableValue<QDateTime>(*this);
            break;

        case ModbusKsuString:
            result = new ModbusStringDeviceVariableValue(*this, precision());
            break;

        case UInt16:
        default:
            result = new ModbusDeviceVariableValue<quint16>(*this);
            break;
    }

    return result;
}

void ModbusVariable::precisionChange()
{
    if (_deviceDataType == ModbusKsuString && _variableValue) {
        ModbusStringDeviceVariableValue *stringVariableValue = dynamic_cast<ModbusStringDeviceVariableValue *>(_variableValue);
        if (stringVariableValue) {
            stringVariableValue->setRegisterCountStringLength(precision());
        }
    }
}

QByteArray ModbusVariable::convertByteOrder(const QByteArray &bytes) const
{
    if (_byteOrder == BigEndian) {
        QByteArray result(bytes);

        int size = bytes.size();
        for (int i = 0; i < size; i++) {
            result[i] = bytes[size - i - 1];
        }
        return result;
    } else if (_byteOrder == PdpEndian && bytes.size() % 2 == 0) {
        QByteArray result(bytes);

        int size = bytes.size();
        for (int i = 0; i < size; i = i + 2) {
            result[i] = bytes[size - i - 2];
            result[i + 1] = bytes[size - i - 1];
        }
        return result;
    }

    return bytes;
}
