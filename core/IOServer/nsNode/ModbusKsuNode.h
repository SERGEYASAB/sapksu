#ifndef MODBUSKSUNODE_H
#define MODBUSKSUNODE_H

#include "Common/PrefixHeader.h"
#include "ModbusRtuNode.h"
#include "ModbusKsuArchive/IModbusKsuArchive.h"

class ModbusVariable;

typedef enum : uint8_t {
    ModbusKsuPasswordLevel1     = 0,
    ModbusKsuPasswordLevel2     = 1,
    ModbusKsuPasswordLevelEnd
} ModbusKsuPasswordLevel;

typedef enum : uint8_t {
    MODBUS_KSU_READ_PASSWORD    = 0x30,
    MODBUS_KSU_WRITE_PASSWORD   = 0x31,
} ModbusKsuPasswordOperation;

class ModbusKsuNode : public ModbusRtuNode
{
    Q_OBJECT

public:
    ModbusKsuNode(int ksuSoftwareVersion, int ksuAddress, QObject *parent = nullptr);
    ~ModbusKsuNode();

public:
    bool readVariable(IVariable *variable, OperationTimingType timingType);
    bool writeVariable(IVariable *variable, const QVariant &writedValue, OperationTimingType timingType);

    ModbusVariable *variableByAddress(quint16 address);

public:
    QList<ModbusVariable *> commonVariables() const;
    inline QMultiMap<quint16, ModbusVariable *> variablesMap() const;

public: /**< архивы */
    IModbusKsuArchive *readArchive(IModbusKsuArchive::ModbusKsuArchiveType type, bool *ok);
    bool moveArchiveRecordToZeroPosition(IModbusKsuArchive::ModbusKsuArchiveType type);
    bool moveArchiveRecordToNextPosition(IModbusKsuArchive::ModbusKsuArchiveType type);
    bool moveArchiveRecordToNPosition(quint16 n, IModbusKsuArchive::ModbusKsuArchiveType type);

public: /**< пароли */
    uint16_t readPassword(ModbusKsuPasswordLevel level, bool *ok = nullptr);
    bool writePassword(ModbusKsuPasswordLevel level, uint16_t password = 0);


public: ///< Методы для получения расшифровки из архивов
    QString eventClassName(int code) const;
    QString eventCodeName(int code) const;
    QString kindOfLaunchName(int code) const;
    QString stopReasonName(int code) const;

    // INode interface
protected:
    QList<SharedPeriodicDataTaskPtr> makePeriodicReadTasks() override;

    // INode interface
protected slots:
    void writeValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void writeValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void readValueSync(IVariable *variable) override;

protected:
    IModbusKsuArchive *parseArchiveResponse(const QByteArray &tx,
                                            const QByteArray &rx,
                                            IModbusKsuArchive::ModbusKsuArchiveType type);

    bool parseReadPasswordResponse(const QByteArray &tx, const QByteArray &rx, uint16_t *password = nullptr);
    bool parseWritePasswordResponse(const QByteArray &tx, const QByteArray &rx);


protected:
    QByteArray moveArchiveRecordToZeroPositionBytes(IModbusKsuArchive::ModbusKsuArchiveType archiveType) const;
    QByteArray moveArchiveRecordToNextPositionBytes(IModbusKsuArchive::ModbusKsuArchiveType archiveType) const;
    QByteArray moveArchiveRecordToNPositionBytes(quint16 n, IModbusKsuArchive::ModbusKsuArchiveType archiveType) const;

    QByteArray readArchiveRecordBytes(IModbusKsuArchive::ModbusKsuArchiveType archiveType) const;

    QByteArray readPasswordBytes(ModbusKsuPasswordLevel level) const;
    QByteArray writePasswordBytes(ModbusKsuPasswordLevel level, uint16_t password) const;

private:
    void loadSoftwareVersion();
    void loadVariables(const QString &variablesConfigPath);
    void traverseXMLVariables(const QDomNode &domNode);
    ModbusVariable *parseVariable(const QDomElement &domElement);

    void setupKindOfLaunch(int comProtocolCode);
    void setupStopReasonCodes(int comProtocolCode);
    void setupModbusArchivesEventClasses(int comProtocolCode);
    void setupModbusArchivesEventCodes(int comProtocolCode);

    static void writeCallback(IVariable *variable, const QVariant &writedValue, const QByteArray &tx, const QByteArray &rx);
    static void readCallback(IVariable *variable, const QByteArray &tx, const QByteArray &rx);

private:
    int m_ksuSoftwareVersion = -1;

    QMultiMap<quint16, ModbusVariable *> m_variablesMap;

    QHash<quint64, QString> m_kindOfLaunchCodes;
    QHash<quint64, QString> m_stopReasonCodes;
    QHash<quint64, QString> m_archiveEventClasses;
    QHash<quint64, QString> m_archiveEventCodes;
};

inline QMultiMap<quint16, ModbusVariable *> ModbusKsuNode::variablesMap() const
{ return m_variablesMap; }

#endif // MODBUSKSUNODE_H
