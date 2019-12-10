#ifndef MODBUSRTUNODE_H
#define MODBUSRTUNODE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsNode/ModbusNode.h"

class MrtuTaskStrategy;

class ModbusRtuNode : public ModbusNode
{
    Q_OBJECT

public:
    explicit ModbusRtuNode(QObject *parent = nullptr);
    explicit ModbusRtuNode(QDomElement &node, QObject *parent = nullptr);

protected:
    bool validateResponse(const QByteArray &tx, const QByteArray &rx) const override;
    QByteArray getModbusAdu(const QByteArray &modbusPdu) const override;
    QByteArray getModbusPdu(const QByteArray &modbusAdu) const override;
};

#endif // MODBUSRTUNODE_H
