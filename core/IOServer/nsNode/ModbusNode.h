#ifndef IMODBUSNODE_H
#define IMODBUSNODE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsNode/INode.h"

class ModbusVariable;

typedef void (*WriteValueCallback)(IVariable *variable, const QVariant &writedValue, const QByteArray &tx, const QByteArray &rx);
typedef void (*ReadValueCallback)(IVariable *variable, const QByteArray &tx, const QByteArray &rx);

class ModbusNode : public INode
{
    Q_OBJECT

    Q_PROPERTY(quint8 address READ address WRITE setAddress)
    Q_PROPERTY(ModbusReadFunction readFunction READ readFunction WRITE setReadFunction)
    Q_PROPERTY(ModbusWriteFunction writeFunction READ writeFunction WRITE setWriteFunction)
    Q_PROPERTY(bool groupRequest READ groupRequest WRITE setGroupRequest)
    Q_PROPERTY(quint8 hollowRegistersCount READ hollowRegistersCount WRITE setHollowRegistersCount) //количество незначащих регистров
    Q_PROPERTY(quint8 maxLengthPdu READ maxLengthPdu WRITE setMaxLengthPdu)
    Q_PROPERTY(QString relayAddresses READ relayAddresses WRITE setRelayAddresses)
    Q_PROPERTY(QString firstCommingAddresses READ firstCommingAddresses WRITE setFirstCommingAddresses)

public:
    enum ModbusReadFunction {
        ReadCoils               = 0x01,
        ReadDiscreteInputs      = 0x02,
        ReadHoldingRegisters    = 0x03,
        ReadInputRegister       = 0x04,
    };
    Q_ENUM(ModbusReadFunction)

    enum ModbusWriteFunction {
        WriteSingleCoil         = 0x05,
        WriteSingleRegister     = 0x06,
        WriteMultipleCoils      = 0x0F,
        WriteMultipleRegisters  = 0x10
    };
    Q_ENUM(ModbusWriteFunction)

    enum ModbusRequestField {
        Address         = 0x00,
        Function        = 0x01,
        StartAddressHi  = 0x02,
        StartAddressLo  = 0x03,
        QuantityHi      = 0x04,
        QuantityLo      = 0x05,
        BytesCount      = 0x06
    };
    Q_ENUM(ModbusRequestField)

    enum ModbusError {
        IllegalFunction             = 0x01,
        IllegalDataAddress          = 0x02,
        IllegalDataValue            = 0x03,
        DeviceFailure               = 0x04,
        DeviceProcessing            = 0x05,
        DeviceIsBusy                = 0x06,
        FunctionCannotPerformed     = 0x07,
        DeviceMemoryParityError     = 0x08,
//        GatewayPathUnavailable      = 0x0A,
//        GatewayTargetDeviceFailed   = 0x0B
    };
    Q_ENUM(ModbusError)

    explicit ModbusNode(QObject *parent = nullptr);
    explicit ModbusNode(QDomElement &node, QObject *parent = nullptr);

    // адрес переменной
    quint8 address() const noexcept;
    void setAddress(quint8 address) noexcept;

    // функция чтения
    ModbusReadFunction readFunction() const;
    void setReadFunction(const ModbusReadFunction &readFunction);

    // функция записи
    ModbusWriteFunction writeFunction() const;
    void setWriteFunction(const ModbusWriteFunction &writeFunction);

    bool groupRequest() const noexcept;
    void setGroupRequest(bool groupRequest) noexcept;

    quint8 hollowRegistersCount() const noexcept;
    void setHollowRegistersCount(quint8 hollowRegistersCount) noexcept;

    quint8 maxLengthPdu() const noexcept;
    void setMaxLengthPdu(quint8 maxLengthPdu) noexcept;

    QString relayAddresses() const noexcept;
    void setRelayAddresses(const QString &relayAddresses) noexcept;

    QString firstCommingAddresses() const noexcept;
    void setFirstCommingAddresses(const QString &firstCommingAddresses) noexcept;

    void appendVariable(IVariable *variable) override;

protected slots:
    bool parseResponse(SharedDataTaskPtr task, const QByteArray &rx) override;

    void writeValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void writeValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void readValueSync(IVariable *variable) override;

protected:
    QList<SharedPeriodicDataTaskPtr> makePeriodicReadTasks() override;
    QMap<quint32, QMap<quint16, ModbusVariable *>> sortModbusMap() const;

    virtual bool validateResponse(const QByteArray &tx, const QByteArray &rx) const = 0;

    virtual QByteArray getModbusAdu(const QByteArray &modbusPdu) const = 0;
    virtual QByteArray getModbusPdu(const QByteArray &modbusAdu) const = 0;

    void setWriteValueCallback(WriteValueCallback callback);
    void setReadValueCallback(ReadValueCallback callback);

protected:
    QByteArray readRegistersRequestBytes(quint8 function, quint16 registerAddress, quint16 registersCount) const;
    QByteArray writeSingleRegisterBytes(quint8 function, quint16 registerAddress, quint16 value) const;
    QByteArray writeSingleRegisterBytes(quint8 function, quint16 registerAddress, const QByteArray &bytes) const;
    QByteArray writeMultipleRegistersBytes(quint8 function, quint16 registerAddress, quint16 value) const;
    QByteArray writeMultipleRegistersBytes(quint8 function, quint16 registerAddress, quint16 registersCount, const QByteArray &bytes) const;

    QByteArray readVariableBytes(const ModbusVariable *variable) const;
    QByteArray readVariableBytes(quint8 function, const ModbusVariable *variable) const;

    QByteArray writeVariableBytes(const ModbusVariable *variable, const QVariant &value, const QByteArray &bytes) const;
    QByteArray writeVariableBytes(quint8 function, const ModbusVariable *variable, const QVariant &value, const QByteArray &bytes) const;

    QList<ModbusVariable *> variablesByTx(const QByteArray &tx) const;
    void setVariablesError(const QList<ModbusVariable *> &variables, const NodeError &error);

    const QHash<int, QString> &protocolErrors() const;

private:
    QByteArray dataForWriteValueTask(ModbusVariable *variable, const QVariant &value, const QByteArray &bytes) const;

    quint8 _address = 0;
    ModbusReadFunction _readFunction = ReadHoldingRegisters;
    ModbusWriteFunction _writeFunction = WriteMultipleRegisters;
    bool _groupRequest = true;
    quint8 _hollowRegistersCount = 1;
    quint8 _maxLengthPdu = 250;
    QString _firstCommingAddresses;
    QString _relayAddresses;

    QHash<int, QString> m_protocolErrors;

    WriteValueCallback m_writeCallback = nullptr;
    ReadValueCallback m_readCallback = nullptr;
};

#endif // MODBUSNODE_H
