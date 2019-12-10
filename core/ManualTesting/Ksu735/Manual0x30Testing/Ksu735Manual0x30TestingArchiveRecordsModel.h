#ifndef KSU735MANUAL0X30TESTINGARCHIVERECORDSMODEL_H
#define KSU735MANUAL0X30TESTINGARCHIVERECORDSMODEL_H

#include "Common/PrefixHeader.h"
#include "IOServer/NodeError/NodeError.h"
#include "IOServer/nsNode/Ksu735NodeArchive.h"

struct Ksu735Manual0x30TestingArchiveRecord
{
    int number = 0;
    Ksu735NodeArchive *archiveRecord = nullptr;
};

class Ksu735Manual0x30TestingArchiveRecordsModel
{
public:
    inline QList<Ksu735Manual0x30TestingArchiveRecord> records() const;
    inline void setRecords(const QList<Ksu735Manual0x30TestingArchiveRecord> &records);
    inline void clean();

    inline NodeError lastError() const;
    inline void setLastError(const NodeError &lastError);

    inline bool hasError() const;

private:
    QList<Ksu735Manual0x30TestingArchiveRecord> m_records;
    NodeError m_lastError = NodeError::noError();
};

inline QList<Ksu735Manual0x30TestingArchiveRecord> Ksu735Manual0x30TestingArchiveRecordsModel::records() const
{
    return m_records;
}

inline void Ksu735Manual0x30TestingArchiveRecordsModel::setRecords(const QList<Ksu735Manual0x30TestingArchiveRecord> &records)
{
    m_records = records;
}

inline void Ksu735Manual0x30TestingArchiveRecordsModel::clean()
{
    for (auto record : m_records) {
        delete record.archiveRecord;
    }
    m_records.clear();
    m_lastError = NodeError::noError();
}

inline NodeError Ksu735Manual0x30TestingArchiveRecordsModel::lastError() const
{
    return m_lastError;
}

inline void Ksu735Manual0x30TestingArchiveRecordsModel::setLastError(const NodeError &lastError)
{
    m_lastError = lastError;
}

inline bool Ksu735Manual0x30TestingArchiveRecordsModel::hasError() const
{
    return m_lastError != NodeError::noError();
}

#endif // KSU735MANUAL0X30TESTINGARCHIVERECORDSMODEL_H
