#ifndef MODBUSDEVICEVARIABLEVALUE_H
#define MODBUSDEVICEVARIABLEVALUE_H

#include "Common/PrefixHeader.h"
#include "DeviceVariableValueBase.h"

template <typename T>
struct ModbusValueConversion
{
    static T convertBytesToType(const QByteArray &bytes) {
        T result = 0;
        quint8 bytesCount = static_cast<quint8>(bytes.length());
        for (qint8 pos = bytesCount - 1, offset = 0; pos >= 0; pos--, offset += 8) {
            result |= quint64(quint8(bytes[pos])) << offset;
        }
        return result;
    }

    static QByteArray convertValueToBytes(const T &value) {
        QByteArray bytes;
        bytes.reserve(sizeof(T) - 1);
        for (qint8 pos = sizeof(T) - 1, offset = 0; pos >= 0; pos--, offset += 8)
            bytes[pos] = (value >> offset);
        return bytes;
    }
};


/// Float specialization
template <>
struct ModbusValueConversion<float> {
    static float convertBytesToType(const QByteArray &bytes) {
        quint32 int32value = ModbusValueConversion<quint32>::convertBytesToType(bytes);
        float result = Sap::raw_cast<float>(int32value);
        return result;
    }

    static QByteArray convertValueToBytes(float value) {
        quint32 int32value = Sap::raw_cast<quint32>(value);
        return ModbusValueConversion<quint32>::convertValueToBytes(int32value);
    }
};

/// Double specialization
template <>
struct ModbusValueConversion<double> {
    static double convertBytesToType(const QByteArray &bytes) {
        quint64 int64value = ModbusValueConversion<quint64>::convertBytesToType(bytes);
        double result = Sap::raw_cast<double>(int64value);
        return result;
    }

    static QByteArray convertValueToBytes(double value) {
        quint64 int64value = Sap::raw_cast<quint64>(value);
        return ModbusValueConversion<quint64>::convertValueToBytes(int64value);
    }
};


/// Ksu date specialization
template <>
struct ModbusValueConversion<QDateTime> {
    static QDateTime convertBytesToType(const QByteArray &bytes) {
        int year = static_cast<quint8>(bytes[0]) + 1900;
        quint8 month = static_cast<quint8>(bytes[1]);
        quint8 day = static_cast<quint8>(bytes[2]);
        quint8 hour = static_cast<quint8>(bytes[3]);
        quint8 minutes = static_cast<quint8>(bytes[4]);
        quint8 seconds = static_cast<quint8>(bytes[5]);

        QDate date(year, month, day);
        QTime time(hour, minutes, seconds);
        return QDateTime(date, time);
    }

    static QByteArray convertValueToBytes(const QDateTime &value) {
        QByteArray bytes;
        bytes.append(static_cast<char>(value.date().year() - 1900));
        bytes.append(static_cast<char>(value.date().month()));
        bytes.append(static_cast<char>(value.date().day()));
        bytes.append(static_cast<char>(value.time().hour()));
        bytes.append(static_cast<char>(value.time().minute()));
        bytes.append(static_cast<char>(value.time().second()));

        return bytes;
    }
};


/// Ksu string specialization
template <>
struct ModbusValueConversion<QString> {
    static QString convertBytesToType(const QByteArray &bytes, int length) {
        QByteArray cp866Bytes = bytes;
        if (cp866Bytes.size() > length) {
            cp866Bytes.resize(length);
        }

        QByteArray convertedBytes = cp866Bytes;
        for (int i = 0; i < bytes.size(); i += 2) {
            convertedBytes[i] = cp866Bytes[i+1];
            convertedBytes[i+1] = cp866Bytes[i];
        }

        auto decoder = QTextCodec::codecForName("cp866")->makeDecoder();
        return decoder->toUnicode(convertedBytes);
    }

    static QString convertBytesToType(const QByteArray &bytes) {
        return convertBytesToType(bytes, bytes.size());
    }

    static QByteArray convertValueToBytes(const QString &value, int length) {
        auto encoder = QTextCodec::codecForName("cp866")->makeEncoder();
        QByteArray bytes = encoder->fromUnicode(value);

        if (bytes.size() < length) {
            bytes.append(length - bytes.size(), 0);
        } else if (bytes.size() > length) {
            bytes.resize(length);
        }

        QByteArray convertedBytes = bytes;
        for (int i = 0; i < bytes.size(); i += 2) {
            convertedBytes[i] = bytes[i+1];
            convertedBytes[i+1] = bytes[i];
        }

        return convertedBytes;
    }

    static QByteArray convertValueToBytes(const QString &value) {
        return convertValueToBytes(value, value.size());
    }
};


template <typename T>
class ModbusDeviceVariableValue : public DeviceVariableValueBase<T>
{
public:
    ModbusDeviceVariableValue(IVariable &variable) : DeviceVariableValueBase<T>(variable) {}

    bool setValueBytes(const QByteArray &bytes) override
    {
        if (bytes.count() != this->bytesCount()) {
            return false;
        }

        T result = ModbusValueConversion<T>::convertBytesToType(bytes);
        this->setValue(result);
        return true;
    }

    QByteArray bytesFromValue(const QVariant &variantValue) const override {
        if (!variantValue.canConvert<T>())
            return QByteArray();

        T value = variantValue.value<T>();
        return ModbusValueConversion<T>::convertValueToBytes(value);
    }

    QByteArray currentValueBytes() const override {
        if (!this->currentValue())
            return QByteArray();

        return ModbusValueConversion<T>::convertValueToBytes(*this->currentValue());
    }

    quint8 bytesCount() const override {
        return DeviceVariableValueBase<T>::bytesCount();
    }
};


class ModbusStringDeviceVariableValue : public DeviceVariableValueBase<QString>
{
public:
    ModbusStringDeviceVariableValue(IVariable &variable, uint registerCount = 0) :
        DeviceVariableValueBase<QString>(variable),
        m_registerCountStringLength(registerCount) {}

    bool setValueBytes(const QByteArray &bytes) override
    {
        if (bytes.count() != this->bytesCount()) {
            return false;
        }

        QString result = ModbusValueConversion<QString>::convertBytesToType(bytes);
        this->setValue(result);
        return true;
    }

    QByteArray bytesFromValue(const QVariant &variantValue) const override {
        if (!variantValue.canConvert<QString>())
            return QByteArray();

        QString value = variantValue.value<QString>();
        return ModbusValueConversion<QString>::convertValueToBytes(value, stringLength());
    }

    QByteArray currentValueBytes() const override {
        if (!this->currentValue())
            return QByteArray();

        return ModbusValueConversion<QString>::convertValueToBytes(*this->currentValue(), stringLength());
    }

    quint8 bytesCount() const override {
        return stringLength();
    }

    quint8 registersCount() const override {
        return m_registerCountStringLength;
    }

    inline uint registerCountStringLength() const noexcept {
        return m_registerCountStringLength;
    }

    inline void setRegisterCountStringLength(uint registerCountStringLength) noexcept {
        m_registerCountStringLength = registerCountStringLength;
    }

private:
    inline int stringLength() const noexcept {
        return m_registerCountStringLength * 2;
    }

    uint m_registerCountStringLength = 0;
};


///NOTE: template member function specialization
template <>
quint8 ModbusDeviceVariableValue<bool>::bytesCount() const;

template <>
quint8 ModbusDeviceVariableValue<QDateTime>::bytesCount() const;



#endif // MODBUSDEVICEVARIABLEVALUE_H
