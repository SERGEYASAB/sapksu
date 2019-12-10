#ifndef MODBUSKSUCHANGEPARAMETRSARCHIVE_H
#define MODBUSKSUCHANGEPARAMETRSARCHIVE_H

#include "Common/PrefixHeader.h"
#include "IModbusKsuArchive.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/ModbusDeviceVariableValue.h"

class ModbusVariable;

class ModbusKsuChangeParametrsArchive : public IModbusKsuArchive
{
    Q_OBJECT

    Q_PROPERTY(quint32 parametrAdress READ parametrAdress)
    Q_PROPERTY(float oldValue READ oldValue)
    Q_PROPERTY(float newValue READ newValue)
    Q_PROPERTY(ParametrChangeType changeType READ changeType)
    Q_PROPERTY(QString parametrName READ parametrName)


public:
    enum ParametrChangeType {
        FirstLevelPassword = 0,
        SecondLevelPassword = 1,
        Operator = 2,
        Automatically = 3,
        Remotely = 255
    };
    Q_ENUM(ParametrChangeType)

    explicit ModbusKsuChangeParametrsArchive(ModbusKsuNode *node, const QByteArray &recordBytes, QObject *parent = nullptr);

    // IModbusKsuArchive interface
public:
    ModbusKsuArchiveType type() const override;

public:
    inline qint16 parametrAdress() const { return m_parametrAdress; }
    inline float oldValue() const { return m_oldValue; }
    inline float newValue() const { return m_newValue; }
    inline ParametrChangeType changeType() const { return static_cast<ParametrChangeType>(m_changeType); }

    ModbusVariable *parametr() const;
    QString parametrName() const;

    QString changeTypeString() const;

private:
    friend class ModbusKsuNode;

    qint16 m_parametrAdress = -1;
    float m_oldValue = 0;
    float m_newValue = 0;
    qint16 m_changeType = 0;
};

#endif // MODBUSKSUCHANGEPARAMETRSARCHIVE_H
