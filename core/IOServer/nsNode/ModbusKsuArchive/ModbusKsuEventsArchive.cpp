#include "ModbusKsuEventsArchive.h"
#include "IOServer/nsNode/ModbusKsuNode.h"
#include "IOServer/nsVariable/ModbusVariable.h"

static const uint ArchiveParametrsLenght = 22;

ModbusKsuEventsArchive::ModbusKsuEventsArchive(ModbusKsuNode *node, const QByteArray &recordBytes, QObject *parent) :
    IModbusKsuArchive(node, parent)
{
    if (recordBytes.size() != ArchiveParametrsLenght)
        return;

    if (recordBytes == QByteArray(ArchiveParametrsLenght, 0)) {
        return;
    }

    if (recordBytes.mid(4, 6) == QByteArray(6, 0)) {
        return;
    }

    setEmpty(false);

    setNumber(                      ModbusValueConversion<quint32>::convertBytesToType(recordBytes.mid(0, 4))   );
    setDate(                        ModbusValueConversion<QDateTime>::convertBytesToType(recordBytes.mid(4, 6)) );
    setEventClass(                  ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(10, 2))   );
    setEventCode(                   ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(12, 2))   );

    m_kindOfLaunchOrStopReason =    ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(14, 2));
    m_stopParametrAdress =          ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(16, 2));
    m_stopParametrValue =           ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(18, 4));
}

IModbusKsuArchive::ModbusKsuArchiveType ModbusKsuEventsArchive::type() const
{
    return IModbusKsuArchive::ModbusKsuArchiveType::EventsArchive;
}

ModbusVariable *ModbusKsuEventsArchive::stopParametr() const
{
    return node()->variableByAddress(m_stopParametrAdress);
}

QString ModbusKsuEventsArchive::stopParametrName() const
{
    if (eventCode() == 512 && m_stopParametrAdress != -1) {
        ModbusVariable *variable = stopParametr();
        QString variableName;
        if (variable) {
            variableName = variable->name();
        } else {
            variableName = "Неизвестная переменная";
        }

        return QString("%1 (%2)").arg(m_stopParametrAdress).arg(variableName);
    }

    return QString::number(m_stopParametrAdress);
}

QString ModbusKsuEventsArchive::kindOfLaunchOrStopReasonString() const
{
    if (eventCode() == 256) {
        return node()->kindOfLaunchName(m_kindOfLaunchOrStopReason);
    } else if (eventCode() == 512) {
        return node()->stopReasonName(m_kindOfLaunchOrStopReason);
    } else {
        return QString::number(m_kindOfLaunchOrStopReason);
    }
}
