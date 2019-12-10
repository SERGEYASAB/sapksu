#ifndef IMODBUSKSUARCHIVE_H
#define IMODBUSKSUARCHIVE_H

#include "Common/PrefixHeader.h"

class ModbusKsuNode;

class IModbusKsuArchive : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint32 number READ number)
    Q_PROPERTY(QDateTime date READ date)
    Q_PROPERTY(qint16 eventClass READ eventClass)
    Q_PROPERTY(qint16 eventCode READ eventCode)

public:
    enum class ModbusKsuArchiveType {
        CurrentParametrsArchive = 3000,
        ChangeParametrsArchive = 4000,
        EventsArchive = 5000,
        LaunchParametrsArchive = 6000
    };
    Q_ENUM(ModbusKsuArchiveType)

    explicit IModbusKsuArchive(ModbusKsuNode *node, QObject *parent = 0);

    virtual ModbusKsuArchiveType type() const = 0;

    inline bool isEmpty() const { return m_empty; }
    inline ModbusKsuNode *node() const { return m_node; }

    inline quint32 number() const { return m_number; }
    inline QDateTime date() const { return m_date; }
    inline qint16 eventClass() const { return m_eventClass; }
    inline qint16 eventCode() const { return m_eventCode; }

    QString eventClassTranscriptString() const;
    QString eventCodeTranscriptString() const;

public: ///static
    static quint16 registersCount(ModbusKsuArchiveType type);
    static quint16 readArchiveStartRegisterAddress(ModbusKsuArchiveType type);
    static quint16 moveArchiveRecordRegisterAddress(ModbusKsuArchiveType type);

protected:
    inline void setEmpty(bool empty) { m_empty = empty; }

    inline void setNumber(quint32 number) { m_number = number; }
    inline void setDate(QDateTime date) { m_date = date; }
    inline void setEventClass(qint16 eventClass) { m_eventClass = eventClass; }
    inline void setEventCode(qint16 eventCode) { m_eventCode = eventCode; }

private:
    friend class ModbusKsuNode;
    ModbusKsuNode *m_node;
    bool m_empty = true;

    quint32 m_number = 0;
    QDateTime m_date = QDateTime();
    qint16 m_eventClass = 0;
    qint16 m_eventCode = 0;
};

#endif // IMODBUSKSUARCHIVE_H
