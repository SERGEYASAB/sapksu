#include "EnergoFormDeviceWidget.h"
#include "ui_EnergoFormDeviceWidget.h"

#include "IOServer/EnergoForm/EnergoFormNode.h"

EnergoFormDeviceWidget::EnergoFormDeviceWidget(EnergoFormNode *node, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnergoFormDeviceWidget),
    m_node(node)
{
    ui->setupUi(this);

    connectUiSignals();
    connectNodeSignals();
}

EnergoFormDeviceWidget::~EnergoFormDeviceWidget()
{
    delete ui;
}

void EnergoFormDeviceWidget::startGeneration()
{
    m_node->uploadSignalProfile();
    m_node->startGenerateSignal();
}

void EnergoFormDeviceWidget::stopGeneration()
{
    m_node->stopGenerateSignal();
}

void EnergoFormDeviceWidget::connectUiSignals()
{
    connect(ui->startGenerationButton, &QPushButton::clicked, this, &EnergoFormDeviceWidget::startGeneration);
    connect(ui->stopGenerationButton, &QPushButton::clicked, this, &EnergoFormDeviceWidget::stopGeneration);
}

void EnergoFormDeviceWidget::connectNodeSignals()
{
    for (auto var : m_node->variables()) {
//        connect()
    }
}

void EnergoFormDeviceWidget::createLineEditValidators()
{
    createUValidator();
    createIValidator();
    createAngleValidator();
}

void EnergoFormDeviceWidget::createUValidator()
{
    QDoubleValidator *uValidator = new QDoubleValidator(this);
    uValidator->setBottom(0);
    uValidator->setTop(m_node->maxU());
    uValidator->setDecimals(3);
    uValidator->setNotation(QDoubleValidator::StandardNotation);

    ui->lineEditUa->setValidator(uValidator);
    ui->lineEditUb->setValidator(uValidator);
    ui->lineEditUc->setValidator(uValidator);
}

void EnergoFormDeviceWidget::createIValidator()
{
    QDoubleValidator *iValidator = new QDoubleValidator(this);
    iValidator->setBottom(0);
    iValidator->setTop(m_node->maxI());
    iValidator->setDecimals(5);
    iValidator->setNotation(QDoubleValidator::StandardNotation);

    ui->lineEditIa->setValidator(iValidator);
    ui->lineEditIb->setValidator(iValidator);
    ui->lineEditIc->setValidator(iValidator);
}

void EnergoFormDeviceWidget::createAngleValidator()
{
    QDoubleValidator *angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(-180.0);
    angleValidator->setTop(180.0);
    angleValidator->setDecimals(2);
    angleValidator->setNotation(QDoubleValidator::StandardNotation);

    ui->lineEditAngleUab->setValidator(angleValidator);
    ui->lineEditAngleUbc->setValidator(angleValidator);
    ui->lineEditAngleUca->setValidator(angleValidator);
    ui->lineEditAngleUIa->setValidator(angleValidator);
    ui->lineEditAngleUIb->setValidator(angleValidator);
    ui->lineEditAngleUIc->setValidator(angleValidator);
}
