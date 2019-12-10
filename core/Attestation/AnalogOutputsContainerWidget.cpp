#include "AnalogOutputsContainerWidget.h"
#include "AnalogOutputWidget.h"
#include "AnalogVoltageOutputWidget.h"

AnalogOutputsContainerWidget::AnalogOutputsContainerWidget(QWidget *parent) : QWidget(parent)
{
    AnalogOutputWidget *input1 = new AnalogOutputWidget(1, "$A17_A7", "$A18_A1", "$A26_V1+", "$A24_1-I");
    AnalogOutputWidget *input2 = new AnalogOutputWidget(2, "$A17_B0", "$A18_A2", "$A26_V2+", "$A24_2-I");
    AnalogOutputWidget *input3 = new AnalogOutputWidget(3, "$A17_B1", "$A18_A3", "$A26_V3+", "$A24_3-I");
    AnalogOutputWidget *input4 = new AnalogOutputWidget(4, "$A17_B2", "$A29_A4", "$A26_V4+", "$A24_4-I");
    AnalogOutputWidget *input5 = new AnalogOutputWidget(5, "$A17_B3", "$A18_A4", "$A26_V5+", "$A24_5-I");
    AnalogOutputWidget *input6 = new AnalogOutputWidget(6, "$A17_B4", "$A18_A5", "$A26_V6+", "$A24_6-I");
    AnalogOutputWidget *input7 = new AnalogOutputWidget(7, "$A17_B5", "$A18_A6", "$A26_V7+", "$A25_1-I");
    AnalogOutputWidget *input8 = new AnalogOutputWidget(8, "$A17_B6", "$A18_A7", "$A26_V8+", "$A25_2-I");
    AnalogOutputWidget *input9 = new AnalogOutputWidget(9, "$A17_B7", "$A18_B0", "$A27_V1+", "$A25_3-I");
    AnalogOutputWidget *input10 = new AnalogOutputWidget(10, "$A18_A0", "$A18_B1", "$A27_V2+", "$A25_4-I");
    AnalogVoltageOutputWidget *input11 = new AnalogVoltageOutputWidget(11, "$A27_V3+");

    QGridLayout *containerLayout = new QGridLayout;
    containerLayout->addWidget(input1, 0, 0);
    containerLayout->addWidget(input2, 1, 0);
    containerLayout->addWidget(input3, 2, 0);
    containerLayout->addWidget(input4, 3, 0);
    containerLayout->addWidget(input5, 4, 0);

    containerLayout->addWidget(input6, 0, 1);
    containerLayout->addWidget(input7, 1, 1);
    containerLayout->addWidget(input8, 2, 1);
    containerLayout->addWidget(input9, 3, 1);
    containerLayout->addWidget(input10, 4, 1);

    QHBoxLayout *analogVoltageOutputLayout = new QHBoxLayout;
    analogVoltageOutputLayout->addStretch(1);
    analogVoltageOutputLayout->addWidget(input11, 1);
    analogVoltageOutputLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(containerLayout);
    mainLayout->addLayout(analogVoltageOutputLayout);
    mainLayout->addStretch(2);

    setLayout(mainLayout);
}
