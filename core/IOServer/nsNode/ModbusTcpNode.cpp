#include "ModbusTcpNode.h"

#include "IOServer/nsTask/IDataTask.h"

quint16 ModbusTcpNode::transactionId = 0x0000;

ModbusTcpNode::ModbusTcpNode(QObject *parent) :
    ModbusNode(parent)
{

}

ModbusTcpNode::ModbusTcpNode(QDomElement &node, QObject *parent) :
    ModbusNode(node, parent)
{
}

bool ModbusTcpNode::validateResponse(const QByteArray &tx, const QByteArray &rx) const
{
    if (rx.length() == 0 || rx.length() < 9) {
//        qInfo() << QString("Запрос: %1").arg(QString(tx.toHex().toUpper())) << "Ответ: устройство не отвечает";
        return false;
    }

    if (QByteArray(tx, 2) == QByteArray(rx, 2)){

        qint16 txLength = (tx[4] << 8) | tx[5];
        qint16 rxLength = (rx[4] << 8) | rx[5];

        if ((txLength == tx.length() - 6) && (rxLength == rx.length() - 6)) {

            return true;
        }
    }

    return false;
}

QByteArray ModbusTcpNode::getModbusPdu(const QByteArray &modbusAdu) const
{
    QByteArray pdu = QByteArray(modbusAdu);
    return pdu.remove(0, 6);
}

QByteArray ModbusTcpNode::getModbusAdu(const QByteArray &modbusPdu) const
{
    QByteArray changedData;

    changedData[0] = transactionId >> 8;
    changedData[1] = transactionId;
    changedData[2] = 0x00;
    changedData[3] = 0x00;
    changedData[4] = modbusPdu.length() >> 8;
    changedData[5] = modbusPdu.length();

    changedData.append(modbusPdu);

    transactionId++;

    return changedData;
}
