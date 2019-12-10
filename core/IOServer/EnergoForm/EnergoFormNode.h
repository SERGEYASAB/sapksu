#ifndef ENERGOFORMNODE_H
#define ENERGOFORMNODE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsNode/INode.h"
#include "IOServer/nsVariable/IVariable.h"

#include "EnergoFormWordVariable.h"
#include "EnergoFormDWordVariable.h"

class EnergoFormVariable;
class EnergoFormWordVariable;
class EnergoFormDWordVariable;

class EnergoFormNode : public INode
{
    Q_OBJECT

public:
    EnergoFormNode(QObject *parent = Q_NULLPTR);

public:
    inline bool signalIsGenerated() const { return m_generated; }

    bool getStatus(OperationTimingType timingType = OperationTimingType::Sync);
    bool startGenerateSignal(OperationTimingType timingType = OperationTimingType::Sync);
    bool stopGenerateSignal(OperationTimingType timingType = OperationTimingType::Sync);
    bool uploadSignalProfile(OperationTimingType timingType = OperationTimingType::Sync);

public:
    inline double maxU() const { return m_maxU; }
    void setMaxU(double maxU);

    inline double maxI() const { return m_maxI; }
    void setMaxI(double maxI);

public: //variables
    inline EnergoFormVariable *variableUa() const { return m_Ua; }
    inline EnergoFormVariable *variableUb() const { return m_Ub; }
    inline EnergoFormVariable *variableUc() const { return m_Uc; }

    inline EnergoFormVariable *variableIa() const { return m_Ia; }
    inline EnergoFormVariable *variableIb() const { return m_Ib; }
    inline EnergoFormVariable *variableIc() const { return m_Ic; }

    inline EnergoFormVariable *variableAngleIa() const { return m_angleIa; }
    inline EnergoFormVariable *variableAngleUb() const { return m_angleUb; }
    inline EnergoFormVariable *variableAngleIb() const { return m_angleIb; }
    inline EnergoFormVariable *variableAngleUc() const { return m_angleUc; }
    inline EnergoFormVariable *variableAngleIc() const { return m_angleIc; }

    inline EnergoFormVariable *variableFrequency() const { return m_frequency; }

protected slots:
    bool parseResponse(SharedDataTaskPtr task, const QByteArray &rx) override;
    void writeValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void writeValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void readValueSync(IVariable *variable) override;

protected: /**< формирование запросов */
    inline static QByteArray statusBytes();
    inline static QByteArray startGenerationBytes();
    inline static QByteArray stopGenerationBytes();

    QByteArray signalProfileBytes() const;

protected:
    QList<SharedPeriodicDataTaskPtr> makePeriodicReadTasks() override;
    bool parseStatusResponse(const QByteArray &rx);

private:
    void createVariables();
    EnergoFormDWordVariable *createUVariable(quint16 offset, const QString &phase, const QVariant &defaultValue = QVariant());
    EnergoFormDWordVariable *createIVariable(quint16 offset, const QString &phase, const QVariant &defaultValue = QVariant());
    EnergoFormWordVariable *createAngleVariable(quint16 offset, const QString &name, int value = 0);
    EnergoFormWordVariable *createFrequencyVariable(quint16 offset = 36);

    bool sendRequest(const QByteArray &request, OperationTimingType timingType);

private:
    QList<EnergoFormVariable *> m_variables;

    bool m_generated = false;

    double m_maxU;
    double m_maxI;

    EnergoFormDWordVariable *m_Ua;
    EnergoFormDWordVariable *m_Ub;
    EnergoFormDWordVariable *m_Uc;

    EnergoFormDWordVariable *m_Ia;
    EnergoFormDWordVariable *m_Ib;
    EnergoFormDWordVariable *m_Ic;

    EnergoFormWordVariable *m_angleIa;
    EnergoFormWordVariable *m_angleUb;
    EnergoFormWordVariable *m_angleIb;
    EnergoFormWordVariable *m_angleUc;
    EnergoFormWordVariable *m_angleIc;

    EnergoFormWordVariable *m_frequency;
};

inline QByteArray EnergoFormNode::statusBytes()
{
    return QByteArray::fromHex("020702000600AB5A");
}

inline QByteArray EnergoFormNode::startGenerationBytes()
{
    return QByteArray::fromHex("02070200000001FC");
}

inline QByteArray EnergoFormNode::stopGenerationBytes()
{
    return QByteArray::fromHex("02070200010032CD");
}

#endif // ENERGOFORMNODE_H
