#include "AnalogInputsContainerWidget.h"
#include "AnalogInputWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"

AnalogInputsContainerWidget::AnalogInputsContainerWidget(QWidget *parent) : QWidget(parent)
{
    AnalogInputWidget *input1 = new AnalogInputWidget(1, "$A18_B3", "$A18_B2", "$A28_CH1+");
    AnalogInputWidget *input2 = new AnalogInputWidget(2, "$A18_B5", "$A18_B4", "$A28_CH2+");
    AnalogInputWidget *input3 = new AnalogInputWidget(3, "$A18_B7", "$A18_B6", "$A28_CH3+");
    AnalogInputWidget *input4 = new AnalogInputWidget(4, "$A19_A1", "$A19_A0", "$A28_CH4+");
    AnalogInputWidget *input5 = new AnalogInputWidget(5, "$A19_A3", "$A19_A2", "$A28_CH5+");

    QVBoxLayout *containerLayout = new QVBoxLayout;
    containerLayout->addWidget(input1);
    containerLayout->addWidget(input2);
    containerLayout->addWidget(input3);
    containerLayout->addWidget(input4);
    containerLayout->addWidget(input5);
    containerLayout->addStretch(1);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(containerLayout);

    setLayout(mainLayout);



    IOServer *ioServer = IOServer::instance();
    if (ioServer) {
        IVariable *disableAnalogInput6Variable = ioServer->variableByAlias("$A19_A5");
        if (disableAnalogInput6Variable) {
            disableAnalogInput6Variable->writeValue(true);
        }
        IVariable *disableAnalogInput7Variable = ioServer->variableByAlias("$A19_A7");
        if (disableAnalogInput7Variable) {
            disableAnalogInput7Variable->writeValue(true);
        }
        IVariable *disableAnalogInput8Variable = ioServer->variableByAlias("$A19_B1");
        if (disableAnalogInput8Variable) {
            disableAnalogInput8Variable->writeValue(true);
        }
    }
}
