#include "ModbusVariableAddressSelectSpinBox.h"

#include "IOServer/nsNode/ModbusKsuNode.h"

ModbusVariableAddressSelectSpinBox::ModbusVariableAddressSelectSpinBox(ModbusKsuNode *node, QWidget *parent) :
    QSpinBox(parent),
    m_node(node)
{
    m_modbusVariableAddresses = m_node->variablesMap().uniqueKeys();
}


//QValidator::State ModbusVariableAddressSelectSpinBox::validate(QString &input, int &pos) const
//{

//}

//void ModbusVariableAddressSelectSpinBox::fixup(QString &input) const
//{

//}

void ModbusVariableAddressSelectSpinBox::stepBy(int steps)
{
    if (qAbs(steps) > m_modbusVariableAddresses.size()) {
        int newSteps = m_modbusVariableAddresses.size() % qAbs(steps);
        if (steps < 0)
            newSteps = -newSteps;
    }

    int index = m_modbusVariableAddresses.indexOf(value());
    if (index != -1) {
        int newIndex = index + steps;
        if (newIndex > m_modbusVariableAddresses.size() - 1) {
            if (wrapping()) {
                newIndex = newIndex - m_modbusVariableAddresses.size();
            } else {
                newIndex = m_modbusVariableAddresses.size() - 1;
            }
        } else if (newIndex < 0) {
            if (wrapping()) {
                newIndex = m_modbusVariableAddresses.size() + newIndex;
            } else {
                newIndex = 0;
            }
        }

        setValue(m_modbusVariableAddresses[newIndex]);
        return;
    }

    if (steps < 0) {
        if (wrapping()) {
            int newIndex = m_modbusVariableAddresses.size() + steps;
            setValue(m_modbusVariableAddresses[newIndex]);
        } else {
            setValue(-1);
        }
    } else {
        setValue(m_modbusVariableAddresses[steps - 1]);
    }
}

int ModbusVariableAddressSelectSpinBox::findNearestVariableAddressIndex(int currentAddress, int step) const
{
    int index = m_modbusVariableAddresses.indexOf(currentAddress);
    if (index == -1) {
        int b = m_modbusVariableAddresses.size() - 1;
        int t = 0;
        for (int i = 0; i < m_modbusVariableAddresses.size(); i++) {
            int address = m_modbusVariableAddresses[i];
//            if (address < m_modbusVariableAddresses[b] && address < )
        }
    }

    return index;
}
