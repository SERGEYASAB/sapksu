#include "EnergoMonitor12Integer20FractionDeviceVariableValue.h"

EnergoMonitor12Integer20FractionDeviceVariableValue::EnergoMonitor12Integer20FractionDeviceVariableValue(IVariable &variable) : DeviceVariableValueBase<double>(variable)
{

}

bool EnergoMonitor12Integer20FractionDeviceVariableValue::setValueBytes(const QByteArray &bytes)
{
    if (bytes.count() != this->bytesCount()) {
        return false;
    }

    quint8 byte0 = static_cast<quint8>(bytes[0]);
    quint8 byte1 = static_cast<quint8>(bytes[1]);
    quint8 byte2 = static_cast<quint8>(bytes[2]);
    quint8 byte3 = static_cast<quint8>(bytes[3]);

    quint32 integerPart = (quint32(byte0) << 4) + (byte1 >> 4);
    quint32 fraction = (quint32(byte1 & 0xF) << 16) + (quint16(byte2) << 8) + byte3;

    double value = integerPart + double(fraction) / 1048576.0;

    this->setValue(value);
    return true;
}

QByteArray EnergoMonitor12Integer20FractionDeviceVariableValue::bytesFromValue(const QVariant &) const
{
    return QByteArray();
}

QByteArray EnergoMonitor12Integer20FractionDeviceVariableValue::currentValueBytes() const
{
    return QByteArray();
}

quint8 EnergoMonitor12Integer20FractionDeviceVariableValue::bytesCount() const
{
    return 4;
}
