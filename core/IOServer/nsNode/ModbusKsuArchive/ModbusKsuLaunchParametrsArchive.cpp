#include "ModbusKsuLaunchParametrsArchive.h"

static const uint ArchiveParametrsLenght = 50;

ModbusKsuLaunchParametrsArchive::ModbusKsuLaunchParametrsArchive(ModbusKsuNode *node, const QByteArray &recordBytes, QObject *parent) :
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

    setNumber(              ModbusValueConversion<quint32>::convertBytesToType(recordBytes.mid(0, 4))   );
    setDate(                ModbusValueConversion<QDateTime>::convertBytesToType(recordBytes.mid(4, 6)) );

    m_milliseconds =        ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(10, 2));

    setEventClass(          ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(12, 2))   );
    setEventCode(           ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(14, 2))   );

    m_Ia =                  ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(16, 4));
    m_Ib =                  ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(20, 4));
    m_Ic =                  ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(24, 4));
    m_IImbalance =          ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(28, 4));
    m_UInputA =             ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(32, 2));
    m_UInputB =             ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(34, 2));
    m_UInputC =             ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(36, 2));
    m_UImbalance =          ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(38, 4));
    m_resistance =          ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(42, 2));
    m_load =                ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(44, 2));
    m_cos =                 ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(46, 4));
}

IModbusKsuArchive::ModbusKsuArchiveType ModbusKsuLaunchParametrsArchive::type() const
{
    return IModbusKsuArchive::ModbusKsuArchiveType::LaunchParametrsArchive;
}
