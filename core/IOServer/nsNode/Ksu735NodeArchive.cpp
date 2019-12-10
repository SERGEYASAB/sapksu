#include "Ksu735NodeArchive.h"

#include "Ksu735Node.h"
#include "IOServer/nsVariable/Ksu735Variable.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/ModbusDeviceVariableValue.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/Ksu735DeviceVariableValue.h"

Ksu735NodeArchive::Ksu735NodeArchive(Ksu735Node *node, const QByteArray &recordBytes, QObject *parent) :
    QObject(parent),
    m_node(node),
    m_recordBytes(recordBytes)
{
    if (recordBytes.size() != 8)
        return;

    m_valid = true;

    if (recordBytes == QByteArray(8, 0)) {
        return;
    }

    m_empty = false;

    quint8 month = Ksu735BcdValueConversion<quint8>::convertBytesToType(m_recordBytes.mid(0, 1));
    quint8 day = Ksu735BcdValueConversion<quint8>::convertBytesToType(m_recordBytes.mid(1, 1));
    quint8 minutes = Ksu735BcdValueConversion<quint8>::convertBytesToType(m_recordBytes.mid(2, 1));
    quint8 hours = Ksu735BcdValueConversion<quint8>::convertBytesToType(m_recordBytes.mid(3, 1));

    QDate date = QDate();
    date.setDate(QDate::currentDate().year(), month, day);

    m_recordDate.setDate(date);
    m_recordDate.setTime(QTime(hours, minutes));

    m_parametrNumber = ModbusValueConversion<quint8>::convertBytesToType(m_recordBytes.mid(4, 1));
    m_stopReasonCode = ModbusValueConversion<quint8>::convertBytesToType(m_recordBytes.mid(5, 1));

    QByteArray parametrValueBytes;
    parametrValueBytes.append(m_recordBytes.at(7)).append(m_recordBytes.at(6));

    m_parametrDeviceValue = Ksu735BcdValueConversion<quint16>::convertBytesToType(parametrValueBytes);

    Ksu735Variable *variable = m_node->variableByAddress(m_parametrNumber);
    if (variable) {
        m_parametrName = variable->name();
        m_parametrValue = variable->valueFromDeviceVariantValue(QVariant::fromValue(m_parametrDeviceValue));
    }

    m_stopReasonName = m_node->stopReasonByCode(m_stopReasonCode);
}
