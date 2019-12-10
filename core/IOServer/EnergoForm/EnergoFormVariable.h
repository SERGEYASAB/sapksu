#ifndef ENERGOFORMVARIABLE_H
#define ENERGOFORMVARIABLE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsVariable/IVariable.h"

class EnergoFormNode;

class EnergoFormVariable : public IVariable
{
    Q_OBJECT

public:
    EnergoFormVariable(QObject *parent = nullptr);

    QVariant readValue() override;
    void writeValue(const QVariant &value) override;
    void setValue(const QVariant &value) override;

    QByteArray valueBytes() const;

    double multiplier() const;
    void setMultiplier(double multiplier);

    inline quint16 offset() const { return m_offset; }
    inline void setOffset(quint16 offset) { m_offset = offset; }

protected:
    QByteArray convertByteOrder(const QByteArray &bytes) const override;

private:
    quint16 m_offset;

    friend class EnergoFormNode;
};

#endif // ENERGOFORMVARIABLE_H
