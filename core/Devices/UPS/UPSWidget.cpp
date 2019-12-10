#include "UPSWidget.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "Devices/Device.h"
#include "StartWindow/ActionStyledBooleanWidget.h"

UPSWidget::UPSWidget(QWidget *parent) : QWidget(parent)
{
    ActionStyledBooleanWidget *m_upsPower = new ActionStyledBooleanWidget(QString("$A23_A3"),
                                                                          ACCESSLEVEL_USER,
                                                                          new QLabel("Питание от ИБП"),
                                                                          QIcon());
    m_upsPower->setWritable(false);

    ActionStyledBooleanWidget *m_electricalNetworkPower = new ActionStyledBooleanWidget(QString("$A23_A3"),
                                                                                        ACCESSLEVEL_USER,
                                                                                        new QLabel("Питание от сети"),
                                                                                        QIcon());
    m_electricalNetworkPower->setWritable(false);
    m_electricalNetworkPower->setInverse(true);

    ActionStyledBooleanWidget *m_batteryCharge = new ActionStyledBooleanWidget(QString("$A23_A3"),
                                                                               ACCESSLEVEL_USER,
                                                                               new QLabel("Заряд АКБ"),
                                                                               QIcon());
    m_batteryCharge->setWritable(false);
    m_batteryCharge->setInverse(true);

    ActionStyledBooleanWidget *m_upsFault = new ActionStyledBooleanWidget(QString("$A23_A0"),
                                                                          ACCESSLEVEL_USER,
                                                                          new QLabel("Неисправность ИБП"),
                                                                          QIcon());
    m_upsFault->setWritable(false);

    ActionStyledBooleanWidget *m_lowBattery = new ActionStyledBooleanWidget(QString("$A23_A1"),
                                                                            ACCESSLEVEL_USER,
                                                                            new QLabel("Низкий заряд батареи"),
                                                                            QIcon());
    m_lowBattery->setWritable(false);

    ActionStyledBooleanWidget *m_upsPowerUp = new ActionStyledBooleanWidget(QString("$A23_A2"),
                                                                               ACCESSLEVEL_USER,
                                                                               new QLabel("ИБП включен"),
                                                                               QIcon());
    m_upsPowerUp->setWritable(false);


    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(m_upsPower);
    leftLayout->addWidget(m_electricalNetworkPower);
    leftLayout->addWidget(m_batteryCharge);
    leftLayout->addStretch(10);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_upsFault);
    rightLayout->addWidget(m_lowBattery);
    rightLayout->addWidget(m_upsPowerUp);
    rightLayout->addStretch(10);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addLayout(leftLayout);
    layout->addLayout(rightLayout);
    layout->addStretch(10);

    setLayout(layout);

    resize(1280, 960);
}
