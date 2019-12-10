#ifndef MODBUSTCPNODE_H
#define MODBUSTCPNODE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsNode/ModbusNode.h"

class MtcpTaskStrategy;

class ModbusTcpNode : public ModbusNode
{
    Q_OBJECT

public:
    explicit ModbusTcpNode(QObject *parent = nullptr);
    explicit ModbusTcpNode(QDomElement &node, QObject *parent = nullptr);

protected:
    bool validateResponse(const QByteArray &tx, const QByteArray &rx) const override;
    QByteArray getModbusPdu(const QByteArray &modbusAdu) const override;
    QByteArray getModbusAdu(const QByteArray &modbusPdu) const override;

private:
    static quint16 transactionId;
};

#endif // MODBUSTCPNODE_H
