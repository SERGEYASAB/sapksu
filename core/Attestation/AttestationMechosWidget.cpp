#include "AttestationMechosWidget.h"
#include "Devices/Circutor/CircutorWidget.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "StartWindow/StartWindow.h"
#include "ReportManager/OperatorClickReport.h"

AttestationMechosWidget::AttestationMechosWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout * hboxLayout = new QHBoxLayout;
    QVBoxLayout *butonLyout = new QVBoxLayout;

    widget = new QWidget;
    widget->setMinimumWidth(350);
    goupLayout = new QVBoxLayout;

    supplyButton = new QPushButton("Подать питаие на СУ");
    startButton = new QPushButton("Пуск СУ");
    loadTen1Button = new QPushButton("Включить тэн 1 ступень");
    loadTen2Button = new QPushButton("Включить тэн 2 ступень");
    loadMotorButton = new QPushButton("Включить тэн 2 ступень и асинх. двигатель");
    loadReactButton = new QPushButton("Включить реактивную нагрузка");

    loadButtons<<loadTen1Button
              <<loadTen2Button
             <<loadMotorButton
            <<loadReactButton;

    for (auto i : loadButtons)
        goupLayout->addWidget(i);
    goupLayout->addStretch();

    widget->setLayout(goupLayout);

    butonLyout->addSpacing(150);
    butonLyout->addWidget(supplyButton);
    butonLyout->addWidget(startButton);
    butonLyout->addWidget(widget);

    connect(supplyButton, &QPushButton::clicked, this, &AttestationMechosWidget::supplyAction);
    connect(startButton, &QPushButton::clicked, this, &AttestationMechosWidget::startAction);
    connect(loadTen1Button, &QPushButton::clicked, this, &AttestationMechosWidget::loadTen1Action);
    connect(loadTen2Button, &QPushButton::clicked, this, &AttestationMechosWidget::loadTen2Action);
    connect(loadMotorButton, &QPushButton::clicked, this, &AttestationMechosWidget::loadMotorAction);
    connect(loadReactButton, &QPushButton::clicked, this, &AttestationMechosWidget::loadReactAction);

    circutorWidget = new CircutorWidget(true);

    hboxLayout->addLayout(butonLyout);
    hboxLayout->addWidget(circutorWidget);

    mainLayout->addWidget(new QLabel("Большинство людей готово безмерно трудиться, лишь бы избавиться от необходимости немножко подумать (Томас Эдисон)"));
    mainLayout->addLayout(hboxLayout);

    setLayout(mainLayout);

}

void AttestationMechosWidget::blockButtons(QPushButton *button)
{
    if (button == startButton) {
        for (auto i : loadButtons)
            i->setEnabled(!i->isEnabled());
        button->setText(button->text().replace(stateOnStartButton ? "Стоп" : "Пуск",
                                               stateOnStartButton ? "Пуск" : "Стоп"));
        stateOnStartButton = !stateOnStartButton;
    } else if (button == loadTen1Button) {
        loadTen2Button->setEnabled(stateOnTen1Button);
        loadMotorButton->setEnabled(stateOnTen1Button);
        loadReactButton->setEnabled(stateOnTen1Button);
        button->setText(button->text().replace(stateOnTen1Button ? "Отключить" : "Включить",
                                               stateOnTen1Button ? "Включить" : "Отключить"));
        circutorWidget->setInvisibleDevice(3,!stateOnTen1Button);
        stateOnTen1Button = !stateOnTen1Button;
    } else if (button == loadTen2Button) {
        loadTen1Button->setEnabled(stateOnTen2Button);
        loadMotorButton->setEnabled(stateOnTen2Button);
        loadReactButton->setEnabled(stateOnTen2Button);
        button->setText(button->text().replace(stateOnTen2Button ? "Отключить" : "Включить",
                                               stateOnTen2Button ? "Включить" : "Отключить"));
        circutorWidget->setInvisibleDevice(3,!stateOnTen2Button);
        stateOnTen2Button = !stateOnTen2Button;
    } else if (button == loadMotorButton) {
        loadTen1Button->setEnabled(stateOnMotorButton);
        loadTen2Button->setEnabled(stateOnMotorButton);
        loadReactButton->setEnabled(stateOnMotorButton);
        button->setText(button->text().replace(stateOnMotorButton ? "Отключить" : "Включить",
                                               stateOnMotorButton ? "Включить" : "Отключить"));
        circutorWidget->setInvisibleDevice(3,!stateOnMotorButton);
        stateOnMotorButton = !stateOnMotorButton;
    } else if (button == loadReactButton) {
        loadTen1Button->setEnabled(stateOnReactButton);
        loadTen2Button->setEnabled(stateOnReactButton);
        loadMotorButton->setEnabled(stateOnReactButton);
        button->setText(button->text().replace(stateOnReactButton ? "Отключить" : "Включить",
                                               stateOnReactButton ? "Включить" : "Отключить"));

        circutorWidget->setInvisibleDevice(2,!stateOnReactButton);
        stateOnReactButton = !stateOnReactButton;
    }
    QApplication::processEvents();
}

void AttestationMechosWidget::supplyAction()
{
    //На случай, если нет связи с оборудованием
    //Лучше блочить до того, как повиснет интерфейс
    blockButtons(supplyButton);

    if (!IOServer::instance())
        return;

    IVariable *state = IOServer::instance()->variableByAlias(QStringLiteral("KSU_STATE_PED")),
            *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE")),
            *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE")),
            *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE")),
            *fcEnable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_POWER_PCH_ENABLE")),
            *outFreq = IOServer::instance()->variableByAlias(QStringLiteral("$UZ1_OUTPUT_FREQ")),
            *fcSupply = IOServer::instance()->variableByAlias(QStringLiteral("$UZ1_CTRL_SUPPLY")),
            *fcDrive = IOServer::instance()->variableByAlias(QStringLiteral("$UZ1_CTRL_DRIVE"));

    if (!state || !phaseA || !phaseB || !phaseC || !fcEnable || !outFreq || !fcSupply || !fcDrive)
        return;

    QVariant value = state->readValue().toBool();

    if (value.isNull() || value.toBool()) {
        phaseA->setValue(false);
        phaseB->setValue(false);
        phaseC->setValue(false);
        fcEnable->setValue(false);
    } else {
        phaseA->setValue(true);
        phaseB->setValue(true);
        phaseC->setValue(true);
        fcEnable->setValue(true);
        outFreq->setValue(50);
        fcSupply->setValue(58);
        fcDrive->setValue(true);
    }
}

void AttestationMechosWidget::startAction()
{
    blockButtons(startButton);

    if (!IOServer::instance())
        return;

    IVariable *state = IOServer::instance()->variableByAlias(QStringLiteral("KSU_STATE_PED")),
            *stateCtrl = IOServer::instance()->variableByAlias(QStringLiteral("KSU_start_stop"));

    if (!state || !stateCtrl)
        return;

    stateCtrl->setValue(!state->readValue().toBool());
}

void AttestationMechosWidget::loadTen1Action()
{
    blockButtons(loadTen1Button);
    if (!IOServer::instance())
        return;

    IVariable *variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN1_ON")),
            *variable2 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

    if (!variable || !variable2)
        return;

    if (variable->readValue().toBool()) {
        variable->setValue(false);
        variable2->setValue(false);
    } else {
        // ################# БЛОК ОТКЛЮЧНИЯ ###################################################

        // Отключаем нагрузку второй ступени
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN2_ON"));

        if (!variable)
            return;

        variable->setValue(false);

        // Отключаем электродвигатель
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_ELECTRIC_MOTOR_ENABLE"));
        if (!variable)
            return;

        variable->setValue(false);

        // ################# БЛОК ВКЛЮЧЕНИЯ ###################################################

        // Включаем тепловую пушку
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

        if (!variable)
            return;

        variable->setValue(true);

        // Отключаем нагрузку первой ступени
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN1_ON"));

        if (!variable)
            return;

        variable->setValue(true);
    }
}

void AttestationMechosWidget::loadTen2Action()
{
    blockButtons(loadTen2Button);

    if (!IOServer::instance())
        return;

    IVariable *variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN2_ON")),
            *variable2 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

    if (!variable || !variable2)
        return;

    if (variable->readValue().toBool()) {
        variable->setValue(false);
        variable2->setValue(false);
    } else {
        // ################# БЛОК ОТКЛЮЧНИЯ ###################################################

        // Отключаем нагрузку 1 ступени
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN1_ON"));

        if (!variable)
            return;

        variable->setValue(false);

        // Отключаем электродвигатель
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_ELECTRIC_MOTOR_ENABLE"));
        if (!variable)
            return;

        variable->setValue(false);

        // ################# БЛОК ВКЛЮЧЕНИЯ ###################################################

        // Включаем тепловую пушку
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

        if (!variable)
            return;

        variable->setValue(true);

        // Отключаем нагрузку 2 ступени
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN2_ON"));

        if (!variable)
            return;

        variable->setValue(true);
    }
}

void AttestationMechosWidget::loadMotorAction()
{
    blockButtons(loadMotorButton);

    if (!IOServer::instance())
        return;

    IVariable *variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_ELECTRIC_MOTOR_ENABLE")),
            *variable1 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN2_ON")),
            *variable2 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

    if (!variable || !variable1 || variable2)
        return;

    if (variable->readValue().toBool()) {
        variable->setValue(false);
        variable1->setValue(false);
        variable2->setValue(false);
    } else {
        // ################# БЛОК ОТКЛЮЧНИЯ ###################################################

        // Отключаем нагрузку 1 ступени
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN1_ON"));

        if (!variable)
            return;

        variable->setValue(false);


        // ################# БЛОК ВКЛЮЧЕНИЯ ###################################################

        // Включаем движок
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

        if (!variable)
            return;

        variable->setValue(true);

        // Включаем нагрузку 2 ступени
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN2_ON"));

        if (!variable)
            return;

        variable->setValue(true);

        // Включаем электродвигатель
        variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_ELECTRIC_MOTOR_ENABLE"));
        if (!variable)
            return;

        variable->setValue(true);
    }
}

void AttestationMechosWidget::loadReactAction()
{
    blockButtons(loadReactButton);
    //Все остальное пиши ПОСЛЕ этой строки
}

void AttestationMechosWidget::closeEvent(QCloseEvent *event)
{
    IOServer::deleteIOServer();

    QWidget::closeEvent(event);
}
