#include "IModbusKsuArchive.h"
#include "IOServer/nsNode/ModbusKsuNode.h"

IModbusKsuArchive::IModbusKsuArchive(ModbusKsuNode *node, QObject *parent) :
    QObject(parent),
    m_node(node)
{

}

QString IModbusKsuArchive::eventClassTranscriptString() const
{
    return m_node->eventClassName(m_eventClass);
}

QString IModbusKsuArchive::eventCodeTranscriptString() const
{
    return m_node->eventCodeName(m_eventCode);
}

quint16 IModbusKsuArchive::registersCount(IModbusKsuArchive::ModbusKsuArchiveType type)
{
    switch (type) {
        case ModbusKsuArchiveType::CurrentParametrsArchive:
            return 51;
            break;

        case ModbusKsuArchiveType::ChangeParametrsArchive:
            return 13;
            break;

        case ModbusKsuArchiveType::EventsArchive:
            return 11;
            break;

        case ModbusKsuArchiveType::LaunchParametrsArchive:
            return 25;
            break;
    }
    return 0;
}

quint16 IModbusKsuArchive::readArchiveStartRegisterAddress(IModbusKsuArchive::ModbusKsuArchiveType type)
{
    switch (type) {
        case ModbusKsuArchiveType::CurrentParametrsArchive:
            return (int)ModbusKsuArchiveType::CurrentParametrsArchive;
            break;

        case ModbusKsuArchiveType::ChangeParametrsArchive:
            return (int)ModbusKsuArchiveType::ChangeParametrsArchive;
            break;

        case ModbusKsuArchiveType::EventsArchive:
            return (int)ModbusKsuArchiveType::EventsArchive;
            break;

        case ModbusKsuArchiveType::LaunchParametrsArchive:
            return (int)ModbusKsuArchiveType::LaunchParametrsArchive;
            break;
    }
    return -1;
}

quint16 IModbusKsuArchive::moveArchiveRecordRegisterAddress(IModbusKsuArchive::ModbusKsuArchiveType type)
{
    switch (type) {
        case ModbusKsuArchiveType::CurrentParametrsArchive:
            return (int)ModbusKsuArchiveType::CurrentParametrsArchive + 900;
            break;

        case ModbusKsuArchiveType::ChangeParametrsArchive:
            return (int)ModbusKsuArchiveType::ChangeParametrsArchive + 900;
            break;

        case ModbusKsuArchiveType::EventsArchive:
            return (int)ModbusKsuArchiveType::EventsArchive + 900;
            break;

        case ModbusKsuArchiveType::LaunchParametrsArchive:
            return (int)ModbusKsuArchiveType::LaunchParametrsArchive + 900;
            break;
    }
    return -1;
}
