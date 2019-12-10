#include "EnergoMonitorSigned3Integer12FractionDeviceVariableValue.h"

EnergoMonitorSigned3Integer12FractionDeviceVariableValue::EnergoMonitorSigned3Integer12FractionDeviceVariableValue(IVariable &variable) : DeviceVariableValueBase<float>(variable)
{

}

bool EnergoMonitorSigned3Integer12FractionDeviceVariableValue::setValueBytes(const QByteArray &bytes)
{
    if (bytes.count() != this->bytesCount()) {
        return false;
    }

    quint8 byte0 = static_cast<quint8>(bytes[0]);
    quint8 byte1 = static_cast<quint8>(bytes[1]);

    float value = 0;
    bool sign = (byte0 & 0x80) >> 7;
    if (sign) {

    }

//    quint32 integerPart = (quint32(byte0) << 4) + (byte1 >> 4);
//    quint32 fraction = (quint32(byte1 & 0xF) << 16) + (quint16(byte2) << 8) + byte3;

//    double value = integerPart + double(fraction) / 1048576.0;

//    this->setValue(result);
    return true;
}

QByteArray EnergoMonitorSigned3Integer12FractionDeviceVariableValue::bytesFromValue(const QVariant &) const
{
    return QByteArray();
}

QByteArray EnergoMonitorSigned3Integer12FractionDeviceVariableValue::currentValueBytes() const
{
    return QByteArray();
}

quint8 EnergoMonitorSigned3Integer12FractionDeviceVariableValue::bytesCount() const
{
    return 2;
}
