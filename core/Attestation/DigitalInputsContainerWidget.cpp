#include "DigitalInputsContainerWidget.h"
#include "DigitalInputWidget.h"

DigitalInputsContainerWidget::DigitalInputsContainerWidget(QWidget *parent) : QWidget(parent)
{
    DigitalInputWidget *input1 = new DigitalInputWidget("Цифровой вход 1", "$A22_A6");
    DigitalInputWidget *input2 = new DigitalInputWidget("Цифровой вход 2", "$A22_A7");
    DigitalInputWidget *input3 = new DigitalInputWidget("Цифровой вход 3", "$A22_B0");
    DigitalInputWidget *input4 = new DigitalInputWidget("Цифровой вход 4", "$A22_B1");
    DigitalInputWidget *input5 = new DigitalInputWidget("Цифровой вход 5", "$A22_B2");
    DigitalInputWidget *input6 = new DigitalInputWidget("Цифровой вход 6", "$A22_B3");
    DigitalInputWidget *input7 = new DigitalInputWidget("Цифровой вход 7", "$A22_B4");
    DigitalInputWidget *input8 = new DigitalInputWidget("Цифровой вход 8", "$A22_B5");
    DigitalInputWidget *input9 = new DigitalInputWidget("Цифровой вход 9", "$A22_B6");
    DigitalInputWidget *input10 = new DigitalInputWidget("Цифровой вход 10", "$A22_B7");


//    QGridLayout *containerLayout = new QGridLayout();
//    containerLayout->setSpacing(10);

//    containerLayout->addWidget(input1, 0, 0);
//    containerLayout->addWidget(input2, 1, 0);
//    containerLayout->addWidget(input3, 2, 0);
//    containerLayout->addWidget(input4, 3, 0);
//    containerLayout->addWidget(input5, 4, 0);
//    containerLayout->addWidget(input6, 0, 1);
//    containerLayout->addWidget(input7, 1, 1);
//    containerLayout->addWidget(input8, 2, 1);
//    containerLayout->addWidget(input9, 3, 1);
//    containerLayout->addWidget(input10, 4, 1);

    QGridLayout *containerLayout = new QGridLayout();
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


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(containerLayout);
    mainLayout->addStretch(1);

    setLayout(mainLayout);
}
