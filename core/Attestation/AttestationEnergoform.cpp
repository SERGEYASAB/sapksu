#include "AttestationEnergoform.h"
#include "Devices/EnergoFormDevice/EnergoFormRunningControl.h"
#include "ReportManager/OperatorClickReport.h"

AttestationEnergoform::AttestationEnergoform(QWidget *parent) : QWidget(parent)
{
    QFont warningLabelFont;
    warningLabelFont.setPointSize(22);

    QLabel *warningLabel = new QLabel();
    warningLabel->setFont(warningLabelFont);
    warningLabel->setAlignment(Qt::AlignCenter);
    warningLabel->setText("Внимание!!! Перед проведением испытания \r\n"
                          "необходимо установить шунт в разъем \"Измер\"");

    QPalette palette = warningLabel->palette();
//    palette.setColor(ui->pLabel->backgroundRole(), Qt::yellow);
    palette.setColor(warningLabel->foregroundRole(), Qt::red);
    warningLabel->setPalette(palette);

    warningLabelFont.setPointSize(14);
    QPushButton *openEnergoformButton = new QPushButton("Открыть программу управления\r\n"
                                                  "Энергоформой / Энергомонитором");
    openEnergoformButton->setFont(warningLabelFont);
    openEnergoformButton->setMinimumHeight(100);
    connect(openEnergoformButton, &QPushButton::clicked, this, &AttestationEnergoform::openEnergoForm);

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->addStretch(1);
    contentLayout->addWidget(warningLabel);
    contentLayout->addSpacing(50);
    contentLayout->addWidget(openEnergoformButton);
    contentLayout->addStretch(2);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addStretch(1);
    mainLayout->addLayout(contentLayout, 1);
    mainLayout->addStretch(1);

    setLayout(mainLayout);
}

void AttestationEnergoform::openEnergoForm()
{
    OperatorClickReport::instance()->write("Открыть окно Энергоформы");

    setWindowState(Qt::WindowMinimized);
    auto energoform = EnergoFormRunningControl::instance();
    energoform->runEnergoForm();
}
