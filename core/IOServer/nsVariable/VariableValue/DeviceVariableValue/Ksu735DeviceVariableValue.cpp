#include "Ksu735DeviceVariableValue.h"

bool Ksu735DeviceVariableValue::setValueBytes(const QByteArray &bytes) {
    if (bytes.count() != this->bytesCount()) {
        return false;
    }

    quint16 result = 0;
    if (m_conversionType == ConversionType::Bin) {
        result = ModbusValueConversion<quint16>::convertBytesToType(bytes);
    } else {
        result = Ksu735BcdValueConversion<quint16>::convertBytesToType(bytes);
    }
    this->setValue(result);
    return true;
}

QByteArray Ksu735DeviceVariableValue::bytesFromValue(const QVariant &variantValue) const {
    if (!variantValue.canConvert<quint16>())
        return QByteArray();

    quint16 value = variantValue.value<quint16>();
    QByteArray bytes;
    if (m_conversionType == ConversionType::Bin) {
        bytes = ModbusValueConversion<quint16>::convertValueToBytes(value);
    } else {
        bytes = Ksu735BcdValueConversion<quint16>::convertValueToBytes(value);
    }
    return bytes;
}

QByteArray Ksu735DeviceVariableValue::currentValueBytes() const {
    if (!currentValue())
        return QByteArray();

    QByteArray bytes;
    if (m_conversionType == ConversionType::Bin) {
        bytes = ModbusValueConversion<quint16>::convertValueToBytes(*currentValue());
    } else {
        bytes = Ksu735BcdValueConversion<quint16>::convertValueToBytes(*currentValue());
    }
    return bytes;
}
