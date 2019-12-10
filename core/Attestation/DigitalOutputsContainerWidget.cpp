#include "DigitalOutputsContainerWidget.h"
#include "DigitalOutputWidget.h"

DigitalOutputsContainerWidget::DigitalOutputsContainerWidget(QWidget *parent) : QWidget(parent)
{
    DigitalOutputWidget *output1 = new DigitalOutputWidget("Цифровой выход 1", "$A16_A3");
    DigitalOutputWidget *output2 = new DigitalOutputWidget("Цифровой выход 2", "$A16_A4");
    DigitalOutputWidget *output3 = new DigitalOutputWidget("Цифровой выход 3", "$A16_A5");
    DigitalOutputWidget *output4 = new DigitalOutputWidget("Цифровой выход 4", "$A16_A6");
    DigitalOutputWidget *output5 = new DigitalOutputWidget("Цифровой выход 5", "$A16_A7");
    DigitalOutputWidget *output6 = new DigitalOutputWidget("Цифровой выход 6", "$A16_B0");
    DigitalOutputWidget *output7 = new DigitalOutputWidget("Цифровой выход 7", "$A16_B1");
    DigitalOutputWidget *output8 = new DigitalOutputWidget("Цифровой выход 8", "$A16_B2");
    DigitalOutputWidget *output9 = new DigitalOutputWidget("Цифровой выход 9", "$A16_B3");
    DigitalOutputWidget *output10 = new DigitalOutputWidget("Цифровой выход 10", "$A16_B4");
    DigitalOutputWidget *output11 = new DigitalOutputWidget("Цифровой выход 11", "$A16_B5");
    DigitalOutputWidget *output12 = new DigitalOutputWidget("Цифровой выход 12", "$A16_B6");
    DigitalOutputWidget *output13 = new DigitalOutputWidget("Цифровой выход 13", "$A16_B7");
    DigitalOutputWidget *output14 = new DigitalOutputWidget("Цифровой выход 14", "$A17_A0");
    DigitalOutputWidget *output15 = new DigitalOutputWidget("Цифровой выход 15", "$A17_A1");
    DigitalOutputWidget *output16 = new DigitalOutputWidget("Цифровой выход 16", "$A17_A2");
    DigitalOutputWidget *output17 = new DigitalOutputWidget("Цифровой выход 17", "$A17_A3");
    DigitalOutputWidget *output18 = new DigitalOutputWidget("Цифровой выход 18", "$A17_A4");
    DigitalOutputWidget *output19 = new DigitalOutputWidget("Цифровой выход 19", "$A17_A5");
    DigitalOutputWidget *output20 = new DigitalOutputWidget("Цифровой выход 20", "$A17_A6");

    QGridLayout *containerLayout = new QGridLayout();
    containerLayout->setSpacing(10);

    containerLayout->addWidget(output1, 0, 0);
    containerLayout->addWidget(output2, 1, 0);
    containerLayout->addWidget(output3, 2, 0);
    containerLayout->addWidget(output4, 3, 0);
    containerLayout->addWidget(output5, 4, 0);
    containerLayout->addWidget(output6, 0, 1);
    containerLayout->addWidget(output7, 1, 1);
    containerLayout->addWidget(output8, 2, 1);
    containerLayout->addWidget(output9, 3, 1);
    containerLayout->addWidget(output10, 4, 1);
    containerLayout->addWidget(output11, 0, 2);
    containerLayout->addWidget(output12, 1, 2);
    containerLayout->addWidget(output13, 2, 2);
    containerLayout->addWidget(output14, 3, 2);
    containerLayout->addWidget(output15, 4, 2);
    containerLayout->addWidget(output16, 0, 3);
    containerLayout->addWidget(output17, 1, 3);
    containerLayout->addWidget(output18, 2, 3);
    containerLayout->addWidget(output19, 3, 3);
    containerLayout->addWidget(output20, 4, 3);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(containerLayout);
    mainLayout->addStretch(1);

    setLayout(mainLayout);
}
