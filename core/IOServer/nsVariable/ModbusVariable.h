 #ifndef MODBUSVARIABLE_H
#define MODBUSVARIABLE_H

#include "IOServer/nsVariable/IVariable.h"

class IDeviceVariableValue;

class ModbusVariable : public IVariable
{
    Q_OBJECT

    Q_PROPERTY(quint16 address READ address WRITE setAddress)
    Q_PROPERTY(ByteOrder byteOrder READ byteOrder WRITE setByteOrder)
    Q_PROPERTY(ModbusDataType deviceDataType READ deviceDataType WRITE setDeviceDataType)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

public:
    enum ModbusDataType {
        Boolean = IVariable::Boolean,
        Int8 = IVariable::Int8,
        UInt8 = IVariable::UInt8,
        Int16 = IVariable::Int16,
        UInt16 = IVariable::UInt16,
        Int32 = IVariable::Int32,
        UInt32 = IVariable::UInt32,
        Int64 = IVariable::Int64,
        UInt64 = IVariable::UInt64,

        Float32 = IVariable::Float32,
        Float64 = IVariable::Float64,

        ModbusKsuDate = IVariable::DateTime,
        ModbusKsuString = IVariable::String
    };
    Q_ENUM(ModbusDataType)

    enum ByteOrder {
        LittleEndian = 0,
        BigEndian,
        PdpEndian,
    };
    Q_ENUM(ByteOrder)

    explicit ModbusVariable(QObject *parent = Q_NULLPTR);

    quint16 address() const;
    void setAddress(quint16 address);

    inline bool isEnabled() const noexcept { return _enabled; }
    inline void setEnabled(bool enabled) noexcept { _enabled = enabled; }

    quint8 registersCount() const;

    // порядок байт
    ByteOrder byteOrder() const;
    void setByteOrder(ByteOrder byteOrder);

    // тип данных
    ModbusDataType deviceDataType() const;
    void setDeviceDataType(ModbusDataType deviceDataType);

    QVariant defaultValue() const override;

protected:
    IDeviceVariableValue *deviceVariableValue() const override {
        return _variableValue;
    }

    virtual QByteArray convertByteOrder(const QByteArray &bytes) const override;
    virtual IDeviceVariableValue *createVariableValue(ModbusDataType deviceDataType);

private slots:
    void precisionChange();

private:
    friend class ModbusNode;

    quint16 _address;

    ModbusDataType _deviceDataType;
    ByteOrder _byteOrder;
    IDeviceVariableValue *_variableValue = nullptr;
    bool _enabled = true;
};

#endif // MODBUSVARIABLE_H
