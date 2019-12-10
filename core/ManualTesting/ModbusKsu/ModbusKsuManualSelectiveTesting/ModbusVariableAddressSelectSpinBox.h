#ifndef MODBUSVARIABLEADDRESSSELECTSPINBOX_H
#define MODBUSVARIABLEADDRESSSELECTSPINBOX_H

#include "Common/PrefixHeader.h"

class ModbusKsuNode;

class ModbusVariableAddressSelectSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    ModbusVariableAddressSelectSpinBox(ModbusKsuNode *node, QWidget *parent = nullptr);

    // QAbstractSpinBox interface
public:
//    QValidator::State validate(QString &input, int &pos) const override;
//    void fixup(QString &input) const override;
    void stepBy(int steps) override;

private:
    int findNearestVariableAddressIndex(int currentAddress, int step) const;

private:
    ModbusKsuNode *m_node;
    QList<quint16> m_modbusVariableAddresses;
};

#endif // MODBUSVARIABLEADDRESSSELECTSPINBOX_H
