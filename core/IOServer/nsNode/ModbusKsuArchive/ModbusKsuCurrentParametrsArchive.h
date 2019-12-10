#ifndef MODBUSKSUCURRENTPARAMETRSARCHIVE_H
#define MODBUSKSUCURRENTPARAMETRSARCHIVE_H

#include "Common/PrefixHeader.h"
#include "IModbusKsuArchive.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/ModbusDeviceVariableValue.h"

class ModbusKsuCurrentParametrsArchive : public IModbusKsuArchive
{
    Q_OBJECT

    Q_PROPERTY(quint16 rotationDirection READ rotationDirection)
    Q_PROPERTY(float frequency READ frequency)
    Q_PROPERTY(float Ia READ Ia)
    Q_PROPERTY(float Ib READ Ib)
    Q_PROPERTY(float Ic READ Ic)
    Q_PROPERTY(float IImbalance READ IImbalance)
    Q_PROPERTY(float Id READ Id)
    Q_PROPERTY(qint16 Ud READ Ud)
    Q_PROPERTY(qint16 UOutputU READ UOutputU)
    Q_PROPERTY(qint16 UOutputV READ UOutputV)
    Q_PROPERTY(qint16 UOutputW READ UOutputW)
    Q_PROPERTY(float activePower READ activePower)
    Q_PROPERTY(float power READ power)
    Q_PROPERTY(float cos READ cos)
    Q_PROPERTY(qint16 load READ load)
    Q_PROPERTY(qint16 UInputA READ UInputA)
    Q_PROPERTY(qint16 UInputB READ UInputB)
    Q_PROPERTY(qint16 UInputC READ UInputC)
    Q_PROPERTY(float UImbalance READ UImbalance)
    Q_PROPERTY(qint16 resistance READ resistance)
    Q_PROPERTY(float pumpInputPressure READ pumpInputPressure)
    Q_PROPERTY(qint16 windingTemp READ windingTemp)
    Q_PROPERTY(qint16 pumpAquaInputTemp READ pumpAquaInputTemp)
    Q_PROPERTY(float XYVibration READ XYVibration)
    Q_PROPERTY(float ZVibration READ ZVibration)
    Q_PROPERTY(float pumpOutputPressure READ pumpOutputPressure)
    Q_PROPERTY(quint16 pumpOutputTemp READ pumpOutputTemp)
    Q_PROPERTY(quint16 pumpOutputQ READ pumpOutputQ)
    Q_PROPERTY(bool door READ door)
    Q_PROPERTY(bool contactManometer READ contactManometer)
    Q_PROPERTY(bool digitalInpit1 READ digitalInpit1)
    Q_PROPERTY(bool digitalInpit2 READ digitalInpit2)
    Q_PROPERTY(bool digitalInpit3 READ digitalInpit3)
    Q_PROPERTY(bool digitalInpit4 READ digitalInpit4)
    Q_PROPERTY(bool digitalInpit5 READ digitalInpit5)
    Q_PROPERTY(bool digitalInpit6 READ digitalInpit6)
    Q_PROPERTY(qint16 controllerTemp READ controllerTemp)


public:
    explicit ModbusKsuCurrentParametrsArchive(ModbusKsuNode *node, const QByteArray &recordBytes, QObject *parent = nullptr);

    // IModbusKsuArchive interface
public:
    ModbusKsuArchiveType type() const override;

public:
    inline qint16 rotationDirection() const { return m_rotationDirection; }
    inline float frequency() const { return m_frequency; }
    inline float Ia() const { return m_Ia; }
    inline float Ib() const { return m_Ib; }
    inline float Ic() const { return m_Ic; }
    inline float IImbalance() const { return m_IImbalance; }
    inline float Id() const { return m_Id; }
    inline quint16 Ud() const { return m_Ud; }
    inline quint16 UOutputU() const { return m_UOutputU; }
    inline quint16 UOutputV() const { return m_UOutputV; }
    inline quint16 UOutputW() const { return m_UOutputW; }
    inline float activePower() const { return m_activePower; }
    inline float power() const { return m_power; }
    inline float cos() const { return m_cos; }
    inline quint16 load() const { return m_load; }
    inline quint16 UInputA() const { return m_UInputA; }
    inline quint16 UInputB() const { return m_UInputB; }
    inline quint16 UInputC() const { return m_UInputC; }
    inline float UImbalance() const { return m_UImbalance; }
    inline quint16 resistance() const { return m_resistance; }
    inline float pumpInputPressure() const { return m_pumpInputPressure; }
    inline quint16 windingTemp() const { return m_windingTemp; }
    inline quint16 pumpAquaInputTemp() const { return m_pumpAquaInputTemp; }
    inline float XYVibration() const { return m_XYVibration; }
    inline float ZVibration() const { return m_ZVibration; }
    inline float pumpOutputPressure() const { return m_pumpOutputPressure; }
    inline quint16 pumpOutputTemp() const { return m_pumpOutputTemp; }
    inline quint16 pumpOutputQ() const { return m_pumpOutputQ; }
    inline bool door() const { return m_door; }
    inline bool contactManometer() const { return m_contactManometer; }
    inline bool digitalInpit1() const { return m_digitalInpit1; }
    inline bool digitalInpit2() const { return m_digitalInpit2; }
    inline bool digitalInpit3() const { return m_digitalInpit3; }
    inline bool digitalInpit4() const { return m_digitalInpit4; }
    inline bool digitalInpit5() const { return m_digitalInpit5; }
    inline bool digitalInpit6() const { return m_digitalInpit6; }
    inline qint16 controllerTemp() const { return m_controllerTemp; }

private:
    friend class ModbusKsuNode;

    qint16 m_rotationDirection = 0;
    float m_frequency = 0;
    float m_Ia = 0;
    float m_Ib = 0;
    float m_Ic = 0;
    float m_IImbalance = 0;
    float m_Id = 0;
    quint16 m_Ud = 0;
    quint16 m_UOutputU = 0;
    quint16 m_UOutputV = 0;
    quint16 m_UOutputW = 0;
    float m_activePower = 0;
    float m_power = 0;
    float m_cos = 0;
    quint16 m_load = 0;
    quint16 m_UInputA = 0;
    quint16 m_UInputB = 0;
    quint16 m_UInputC = 0;
    float m_UImbalance = 0;
    quint16 m_resistance = 0;
    float m_pumpInputPressure = 0;
    quint16 m_windingTemp = 0;
    quint16 m_pumpAquaInputTemp = 0;
    float m_XYVibration = 0;
    float m_ZVibration = 0;
    float m_pumpOutputPressure = 0;
    quint16 m_pumpOutputTemp = 0;
    quint16 m_pumpOutputQ = 0;
    bool m_door = false;
    bool m_contactManometer = false;
    bool m_digitalInpit1 = false;
    bool m_digitalInpit2 = false;
    bool m_digitalInpit3 = false;
    bool m_digitalInpit4 = false;
    bool m_digitalInpit5 = false;
    bool m_digitalInpit6 = false;
    qint16 m_controllerTemp = 0;
};

#endif // MODBUSKSUCURRENTPARAMETRSARCHIVE_H
