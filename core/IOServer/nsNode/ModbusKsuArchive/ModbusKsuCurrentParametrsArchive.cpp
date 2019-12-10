#include "ModbusKsuCurrentParametrsArchive.h"

static const uint ArchiveParametrsLenght = 102;

ModbusKsuCurrentParametrsArchive::ModbusKsuCurrentParametrsArchive(ModbusKsuNode *node,
                                                                   const QByteArray &recordBytes,
                                                                   QObject *parent) :
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
    setEventClass(          ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(10, 2))   );
    setEventCode(           ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(12, 2))   );

    m_rotationDirection =   ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(14, 2));
    m_frequency =           ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(16, 4));
    m_Ia =                  ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(20, 4));
    m_Ib =                  ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(24, 4));
    m_Ic =                  ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(28, 4));
    m_IImbalance =          ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(32, 4));
    m_Id =                  ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(36, 4));
    m_Ud =                  ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(40, 2));
    m_UOutputU =            ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(42, 2));
    m_UOutputV =            ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(44, 2));
    m_UOutputW =            ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(46, 2));
    m_activePower =         ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(48, 4));
    m_power =               ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(52, 4));
    m_cos =                 ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(56, 4));
    m_load =                ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(60, 2));
    m_UInputA =             ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(62, 2));
    m_UInputB =             ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(64, 2));
    m_UInputC =             ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(66, 2));
    m_UImbalance =          ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(68, 4));
    m_resistance =          ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(72, 2));
    m_pumpInputPressure =   ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(74, 4));
    m_windingTemp =         ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(78, 2));
    m_pumpAquaInputTemp =   ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(80, 2));
    m_XYVibration =         ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(82, 4));
    m_ZVibration =          ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(86, 4));
    m_pumpOutputPressure =  ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(90, 4));
    m_pumpOutputTemp =      ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(94, 2));
    m_pumpOutputQ =         ModbusValueConversion<quint16>::convertBytesToType(recordBytes.mid(96, 2));
    m_controllerTemp =      ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(100, 2));


    quint8 digitalParametrs = ModbusValueConversion<quint8>::convertBytesToType(recordBytes.mid(99, 1));


    m_door = digitalParametrs & (1 << 0);
    m_contactManometer = digitalParametrs & (1 << 1);
    m_digitalInpit1 = digitalParametrs & (1 << 2);
    m_digitalInpit2 = digitalParametrs & (1 << 3);
    m_digitalInpit3 = digitalParametrs & (1 << 4);
    m_digitalInpit4 = digitalParametrs & (1 << 5);
    m_digitalInpit5 = digitalParametrs & (1 << 6);
    m_digitalInpit6 = digitalParametrs & (1 << 7);
}

IModbusKsuArchive::ModbusKsuArchiveType ModbusKsuCurrentParametrsArchive::type() const
{
    return IModbusKsuArchive::ModbusKsuArchiveType::CurrentParametrsArchive;
}
