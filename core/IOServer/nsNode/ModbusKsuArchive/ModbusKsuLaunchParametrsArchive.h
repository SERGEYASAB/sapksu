#ifndef MODBUSKSULAUNCHPARAMETRSARCHIVE_H
#define MODBUSKSULAUNCHPARAMETRSARCHIVE_H

#include "Common/PrefixHeader.h"
#include "IModbusKsuArchive.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/ModbusDeviceVariableValue.h"

class ModbusKsuLaunchParametrsArchive : public IModbusKsuArchive
{
    Q_OBJECT

    Q_PROPERTY(quint16 milliseconds READ milliseconds)
    Q_PROPERTY(float Ia READ Ia)
    Q_PROPERTY(float Ib READ Ib)
    Q_PROPERTY(float Ic READ Ic)
    Q_PROPERTY(float IImbalance READ IImbalance)
    Q_PROPERTY(qint16 UInputA READ UInputA)
    Q_PROPERTY(qint16 UInputB READ UInputB)
    Q_PROPERTY(qint16 UInputC READ UInputC)
    Q_PROPERTY(float UImbalance READ UImbalance)
    Q_PROPERTY(qint16 resistance READ resistance)
    Q_PROPERTY(qint16 load READ load)
    Q_PROPERTY(float cos READ cos)


public:
    explicit ModbusKsuLaunchParametrsArchive(ModbusKsuNode *node, const QByteArray &recordBytes, QObject *parent = 0);

    // IModbusKsuArchive interface
public:
    ModbusKsuArchiveType type() const override;

public:
    inline quint16 milliseconds() const { return m_milliseconds; }
    inline float Ia() const { return m_Ia; }
    inline float Ib() const { return m_Ib; }
    inline float Ic() const { return m_Ic; }
    inline float IImbalance() const { return m_IImbalance; }
    inline qint16 UInputA() const { return m_UInputA; }
    inline qint16 UInputB() const { return m_UInputB; }
    inline qint16 UInputC() const { return m_UInputC; }
    inline float UImbalance() const { return m_UImbalance; }
    inline quint16 resistance() const { return m_resistance; }
    inline quint16 load() const { return m_load; }
    inline float cos() const { return m_cos; }

private:
    friend class ModbusKsuNode;

    quint16 m_milliseconds = 0;
    float m_Ia = 0;
    float m_Ib = 0;
    float m_Ic = 0;
    float m_IImbalance = 0;
    quint16 m_UInputA = 0;
    quint16 m_UInputB = 0;
    quint16 m_UInputC = 0;
    float m_UImbalance = 0;
    quint16 m_resistance = 0;
    quint16 m_load = 0;
    float m_cos = 0;
};

#endif // MODBUSKSULAUNCHPARAMETRSARCHIVE_H
