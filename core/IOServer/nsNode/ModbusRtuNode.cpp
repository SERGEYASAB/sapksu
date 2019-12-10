#include "ModbusRtuNode.h"
#include "ModbusCrc.h"

#include "IOServer/nsTask/IDataTask.h"

ModbusRtuNode::ModbusRtuNode(QObject *parent) :
    ModbusNode(parent)
{

}

ModbusRtuNode::ModbusRtuNode(QDomElement &node, QObject *parent) :
    ModbusNode(node, parent)
{
}

bool ModbusRtuNode::validateResponse(const QByteArray &tx, const QByteArray &rx) const
{
    if (rx.length() == 0) {
//        qInfo() << QString("Запрос: %1").arg(QString(tx.toHex().toUpper())) << "Ответ: устройство не отвечает";
        return false;
    }

    if (tx[ModbusRequestField::Address] == rx[ModbusRequestField::Address]) {
        QByteArray crc = ModbusCrc::getInstance()->calcCrc(rx, 0, rx.length() - 2);

        if (crc[0] == rx[rx.length() - 2] && crc[1] == rx[rx.length() - 1]) {
            return true;
        }
    }

    return false;
}

QByteArray ModbusRtuNode::getModbusPdu(const QByteArray &modbusAdu) const
{
    QByteArray pdu = QByteArray(modbusAdu);
    pdu.truncate(pdu.length() - 2);
    return pdu;
}

QByteArray ModbusRtuNode::getModbusAdu(const QByteArray &modbusPdu) const
{
    QByteArray crc = ModbusCrc::getInstance()->calcCrc(modbusPdu, 0, modbusPdu.length());

    QByteArray changedData;
    changedData.append(modbusPdu);
    changedData.append(crc);

    return changedData;
}
