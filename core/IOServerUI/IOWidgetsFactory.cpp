#include "IOWidgetsFactory.h"

#include "IOServer/nsConnection/SerialPortConnection.h"
#include "IOServer/nsConnection/TcpConnection.h"
#include "IOServer/nsNode/ModbusRtuNode.h"
#include "IOServer/nsNode/ModbusTcpNode.h"
#include "IOServer/nsVariable/ModbusVariable.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

#include "Connection/SerialPortConnectionUI.h"
#include "Connection/TcpConnectionUI.h"
#include "Node/ModbusNodeUI.h"
#include "Variable/ModbusVariableUI.h"
#include "Variable/Ksu735VariableUI.h"

QWidget *IOWidgetsFactory::modify(QObject *ptrNode)
{
    if (SerialPortConnection *spc = dynamic_cast<SerialPortConnection *>(ptrNode)) {
        return new SerialPortConnectionUI(spc);
    } else if (TcpConnection *tcp = dynamic_cast<TcpConnection *>(ptrNode)) {
        return new TcpConnectionUI(tcp);
    } else if (ModbusTcpNode *mtcpn = dynamic_cast<ModbusTcpNode *>(ptrNode)) {
        return new ModbusNodeUI(mtcpn);
    } else if (ModbusRtuNode *mrtun = dynamic_cast<ModbusRtuNode *>(ptrNode)) {
        return new ModbusNodeUI(mrtun);
    } else if (ModbusVariable *mbv = dynamic_cast<ModbusVariable *>(ptrNode)) {
        return new ModbusVariableUI(mbv);
    } else if (Ksu735Variable *k7v = dynamic_cast<Ksu735Variable *>(ptrNode)) {
        return new Ksu735VariableUI(k7v);
    }

    return new QLabel(tr("Соединение неизвестного типа"));
}
