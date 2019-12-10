#include "ModbusKsuPasswordsWidget.h"
#include "ui_ModbusKsuPasswordsWidget.h"

#include "IOServer/nsNode/ModbusKsuNode.h"
#include "ErrorClassifier/ErrorClassifier.h"
#include "ReportManager/FaultReport.h"

ModbusKsuPasswordsWidget::ModbusKsuPasswordsWidget(ModbusKsuNode *node, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModbusKsuPasswordsWidget),
    m_node(node)
{
    ui->setupUi(this);

    connect(ui->buttonReadFirstPassword, &QPushButton::clicked, this, &ModbusKsuPasswordsWidget::readFirstPassword);
    connect(ui->buttonWriteFirstPassword, &QPushButton::clicked, this, &ModbusKsuPasswordsWidget::writeFirstPassword);

    connect(ui->buttonReadSecondPassword, &QPushButton::clicked, this, &ModbusKsuPasswordsWidget::readSecondPassword);
    connect(ui->buttonWriteSecondPassword, &QPushButton::clicked, this, &ModbusKsuPasswordsWidget::writeSecondPassword);

    ui->spinBoxFirstPassword->setRange(0, 65535);
    ui->spinBoxSecondPassword->setRange(0, 65535);
}

ModbusKsuPasswordsWidget::~ModbusKsuPasswordsWidget()
{
    delete ui;
}

void ModbusKsuPasswordsWidget::readFirstPassword()
{
    bool ok;
    uint16_t password = m_node->readPassword(ModbusKsuPasswordLevel1, &ok);

    if (!ok) {
        ErrorClassifier::instance()->showError(48);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(48));
        return;
    }

    ui->spinBoxFirstPassword->setValue(password);
}

void ModbusKsuPasswordsWidget::writeFirstPassword()
{
    uint16_t password = static_cast<uint16_t>(ui->spinBoxFirstPassword->value());
    bool result = m_node->writePassword(ModbusKsuPasswordLevel1, password);

    if (!result) {
        ErrorClassifier::instance()->showError(48);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(48));
    }
}

void ModbusKsuPasswordsWidget::readSecondPassword()
{
    bool ok;
    uint16_t password = m_node->readPassword(ModbusKsuPasswordLevel2, &ok);

    if (!ok) {
        ErrorClassifier::instance()->showError(48);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(48));
        return;
    }

    ui->spinBoxSecondPassword->setValue(password);
}

void ModbusKsuPasswordsWidget::writeSecondPassword()
{
    uint16_t password = static_cast<uint16_t>(ui->spinBoxSecondPassword->value());
    bool result = m_node->writePassword(ModbusKsuPasswordLevel2, password);

    if (!result) {
        ErrorClassifier::instance()->showError(48);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(48));
    }
}
