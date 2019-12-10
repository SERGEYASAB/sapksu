#ifndef MODBUSKSUEVENTSARCHIVE_H
#define MODBUSKSUEVENTSARCHIVE_H

#include "Common/PrefixHeader.h"
#include "IModbusKsuArchive.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/ModbusDeviceVariableValue.h"

class ModbusVariable;

class ModbusKsuEventsArchive : public IModbusKsuArchive
{
    Q_OBJECT

    Q_PROPERTY(qint16 kindOfLaunchOrStopReason READ kindOfLaunchOrStopReason)
    Q_PROPERTY(qint16 stopParametrAdress READ stopParametrAdress)
    Q_PROPERTY(float stopParametrValue READ stopParametrValue)
    Q_PROPERTY(QString stopParametrName READ stopParametrName)
    Q_PROPERTY(QString kindOfLaunchOrStopReasonString READ kindOfLaunchOrStopReasonString)


public:
    explicit ModbusKsuEventsArchive(ModbusKsuNode *node, const QByteArray &recordBytes, QObject *parent = 0);

    // IModbusKsuArchive interface
public:
    ModbusKsuArchiveType type() const override;

public:
    inline qint16 kindOfLaunchOrStopReason() const { return m_kindOfLaunchOrStopReason; }
    inline qint16 stopParametrAdress() const { return m_stopParametrAdress; }
    inline float stopParametrValue() const { return m_stopParametrValue; }

    ModbusVariable *stopParametr() const;
    QString stopParametrName() const;

    QString kindOfLaunchOrStopReasonString() const;

private:
    friend class ModbusKsuNode;

    qint16 m_kindOfLaunchOrStopReason = -1;
    qint16 m_stopParametrAdress = -1;
    float m_stopParametrValue = 0;
};

#endif // MODBUSKSUEVENTSARCHIVE_H
