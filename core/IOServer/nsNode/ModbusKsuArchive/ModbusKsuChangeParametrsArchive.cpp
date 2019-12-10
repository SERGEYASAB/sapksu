#include "ModbusKsuChangeParametrsArchive.h"
#include "IOServer/nsNode/ModbusKsuNode.h"
#include "IOServer/nsVariable/ModbusVariable.h"

static const uint ArchiveParametrsLenght = 26;

ModbusKsuChangeParametrsArchive::ModbusKsuChangeParametrsArchive(ModbusKsuNode *node,
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

    m_parametrAdress =      ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(14, 2));
    m_oldValue =            ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(16, 4));
    m_newValue =            ModbusValueConversion<float>::convertBytesToType(recordBytes.mid(20, 4));
    m_changeType =          ModbusValueConversion<qint16>::convertBytesToType(recordBytes.mid(24, 2));
}

IModbusKsuArchive::ModbusKsuArchiveType ModbusKsuChangeParametrsArchive::type() const
{
    return IModbusKsuArchive::ModbusKsuArchiveType::ChangeParametrsArchive;
}

ModbusVariable *ModbusKsuChangeParametrsArchive::parametr() const
{
    return node()->variableByAddress(m_parametrAdress);
}

QString ModbusKsuChangeParametrsArchive::parametrName() const
{
    ModbusVariable *variable = parametr();
    QString variableName;
    if (variable) {
        variableName = variable->name();
    } else {
        variableName = "Неизвестная переменная";
    }

    return QString("%1 (%2)").arg(m_parametrAdress).arg(variableName);
}

QString ModbusKsuChangeParametrsArchive::changeTypeString() const
{
    switch (m_changeType) {
        case FirstLevelPassword:
            return QStringLiteral("По паролю 1 уровня");
            break;

        case SecondLevelPassword:
            return QStringLiteral("По паролю 2 уровня");
            break;

        case Operator:
            return QStringLiteral("Оператор");
            break;

        case Automatically:
            return QStringLiteral("Автоматически");
            break;

        case Remotely:
            return QStringLiteral("Дистанционно");
            break;

        default:
            return QStringLiteral("Неизвестный вид изменения");
            break;
    }
}
