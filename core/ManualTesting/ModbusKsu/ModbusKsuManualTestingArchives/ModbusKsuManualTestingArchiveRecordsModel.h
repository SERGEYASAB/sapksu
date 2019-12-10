#ifndef MODBUSKSUMANUALTESTINGARCHIVERECORDSMODEL_H
#define MODBUSKSUMANUALTESTINGARCHIVERECORDSMODEL_H

#include "Common/PrefixHeader.h"
#include "IOServer/NodeError/NodeError.h"
#include "IOServer/nsNode/ModbusKsuArchive/IModbusKsuArchive.h"

struct ModbusKsuManualTestingArchiveRecord
{
    int number = 0;
    IModbusKsuArchive *archiveRecord = nullptr;
};

class ModbusKsuManualTestingArchiveRecordsModel
{
public:
    inline QList<ModbusKsuManualTestingArchiveRecord> records() const;
    inline void setRecords(const QList<ModbusKsuManualTestingArchiveRecord> &records);
    inline void appendRecord(const ModbusKsuManualTestingArchiveRecord &record);
    inline void appendRecord(IModbusKsuArchive *archiveRecord);
    inline void clean();

    inline NodeError lastError() const;
    inline void setLastError(const NodeError &lastError);

    inline bool hasError() const;

private:
    QList<ModbusKsuManualTestingArchiveRecord> m_records;
    NodeError m_lastError = NodeError::noError();
};

inline QList<ModbusKsuManualTestingArchiveRecord> ModbusKsuManualTestingArchiveRecordsModel::records() const
{
    return m_records;
}

inline void ModbusKsuManualTestingArchiveRecordsModel::setRecords(const QList<ModbusKsuManualTestingArchiveRecord> &records)
{
    m_records = records;
}

inline void ModbusKsuManualTestingArchiveRecordsModel::appendRecord(const ModbusKsuManualTestingArchiveRecord &record)
{
    m_records.append(record);
}

inline void ModbusKsuManualTestingArchiveRecordsModel::appendRecord(IModbusKsuArchive *archiveRecord)
{
    int number = 0;
    if (!m_records.isEmpty()) {
        number = m_records.last().number + 1;
    }

    ModbusKsuManualTestingArchiveRecord record;
    record.number = number;
    record.archiveRecord = archiveRecord;

    m_records.append(record);
}

inline void ModbusKsuManualTestingArchiveRecordsModel::clean()
{
    for (auto record : m_records) {
        delete record.archiveRecord;
    }
    m_records.clear();
    m_lastError = NodeError::noError();
}

inline NodeError ModbusKsuManualTestingArchiveRecordsModel::lastError() const
{
    return m_lastError;
}

inline void ModbusKsuManualTestingArchiveRecordsModel::setLastError(const NodeError &lastError)
{
    m_lastError = lastError;
}

inline bool ModbusKsuManualTestingArchiveRecordsModel::hasError() const
{
    return m_lastError != NodeError::noError();
}

#endif // MODBUSKSUMANUALTESTINGARCHIVERECORDSMODEL_H
