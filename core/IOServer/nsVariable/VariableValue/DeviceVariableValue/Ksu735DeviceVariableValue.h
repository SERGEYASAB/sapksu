#ifndef KSU735DEVICEVARIABLEVALUE_H
#define KSU735DEVICEVARIABLEVALUE_H

#include "Common/PrefixHeader.h"
#include "DeviceVariableValueBase.h"
#include "ModbusDeviceVariableValue.h"

template <typename T>
struct Ksu735BcdValueConversion
{
    static T convertBytesToType(const QByteArray &bytes) {
        T value = 0;
        quint8 bytesCount = static_cast<quint8>(bytes.length());
        for (qint8 pos = bytesCount - 1, offset = 0; pos >= 0; pos--, offset += 8) {
            value |= quint64(quint8(bytes[pos])) << offset;
        }

        T result = QString::number(value, 16).toUInt(0, 10);
        return result;
    }

    static QByteArray convertValueToBytes(const T &value) {
        T bcdValue = QString::number(value).toUShort(0, 16);
        QByteArray bytes;
        bytes.reserve(sizeof(value) - 1);
        for (qint8 pos = sizeof(value) - 1, offset = 0; pos >= 0; pos--, offset += 8) {
            bytes[pos] = (bcdValue >> offset);
        }
        return bytes;
    }
};

template <>
struct Ksu735BcdValueConversion<quint16>
{
    static quint16 convertBytesToType(const QByteArray &bytes) {
        quint16 value = 0;
        quint8 bytesCount = static_cast<quint8>(bytes.length());
        for (qint8 pos = bytesCount - 1, offset = 0; pos >= 0; pos--, offset += 8) {
            value |= quint64(quint8(bytes[pos])) << offset;
        }

        quint16 result = QString::number(value, 16).toUInt(0, 10);
        return result;
    }

    static QByteArray convertValueToBytes(const quint16 &value) {
        quint16 bcdValue = QString::number(value).toUShort(0, 16);
        QByteArray bytes;
        bytes.reserve(sizeof(value) - 1);
        for (qint8 pos = sizeof(value) - 1, offset = 0; pos >= 0; pos--, offset += 8) {
            bytes[pos] = (bcdValue >> offset);
        }
        return bytes;
    }
};

class Ksu735DeviceVariableValue : public DeviceVariableValueBase<quint16>
{
public:
    Ksu735DeviceVariableValue(IVariable &variable) : DeviceVariableValueBase<quint16>(variable) {}

    enum class ConversionType {
        Bin,
        Bcd
    };

    bool setValueBytes(const QByteArray &bytes) override;
    QByteArray bytesFromValue(const QVariant &variantValue) const override;
    QByteArray currentValueBytes() const override;

    inline ConversionType conversionType() const noexcept
    { return m_conversionType; }

    inline void setConversionType(const ConversionType &conversionType) noexcept
    { m_conversionType = conversionType; }

private:
    ConversionType m_conversionType = ConversionType::Bcd;    
};

#endif // KSU735DEVICEVARIABLEVALUE_H
