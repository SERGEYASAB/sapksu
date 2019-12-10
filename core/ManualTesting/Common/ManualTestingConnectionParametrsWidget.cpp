#include "ManualTestingConnectionParametrsWidget.h"
#include "KSUPortConnection/KSUPortConnectionWidget.h"
#include "IOServer/nsConnection/KsuSerialPortConnection.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsNode/ModbusKsuNode.h"
#include "InterferenceSender.h"

#include "ui_ManualTestingConnectionParametrsWidget.h"

ManualTestingConnectionParametrsWidget::ManualTestingConnectionParametrsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualTestingConnectionParametrsWidget)
{
    ui->setupUi(this);

    m_interferenceSender = InterferenceSender::instance();

    QValidator *hexValidator = new QRegExpValidator(QRegExp("^[a-fA-F0-9]+$"), this);
    ui->postfixInterferenceLineEdit->setValidator(hexValidator);
    ui->prefixInterferenceLineEdit->setValidator(hexValidator);

    connect(ui->postfixInterferenceLineEdit, &QLineEdit::textEdited, [this](const QString &text)
    {
        ui->postfixInterferenceLineEdit->setText(text.toUpper());
    });

    connect(ui->prefixInterferenceLineEdit, &QLineEdit::textEdited, [this](const QString &text)
    {
        ui->prefixInterferenceLineEdit->setText(text.toUpper());
    });

#ifdef SAP_KSU
    ui->ethernetGroupBox->hide();
#endif

#ifdef SAP_SU
    ui->ethernetGroupBox->hide();
#endif

#ifdef MECHOS
    cmd = new QProcess;
    connect(cmd, &QProcess::readyReadStandardOutput, [this]()
    {
        ui->ehtLogEdit->append(QTextCodec::codecForName("cp-866")->toUnicode(cmd->readAllStandardOutput().data()).replace("\n",""));
    });

    connect(ui->ethernetLinkTestButton, &QPushButton::clicked, [this]()
    {
        cmd->start("ping", QStringList() << ui->ehtIpEdit->text());
    });
#endif

    m_portSettings = new KSUPortConnectionWidget(this);
    m_portSettings->disablePortName(true);
    m_savePortSettingsButton = new QPushButton(tr("Изменить"));

    m_ksuAddressLabel = new QLabel(tr("Адрес КСУ"));
    m_ksuAddressSpinBox = new QSpinBox();
    m_ksuAddressSpinBox->setMinimum(0);
    m_ksuAddressSpinBox->setMaximum(255);

    m_ksuTimeoutLabel = new QLabel(tr("Таймаут между запросами"));
    m_ksuTimeoutSpinBox = new QSpinBox();
    m_ksuTimeoutSpinBox->setMinimum(0);
    m_ksuTimeoutSpinBox->setMaximum(5000);

    if (Ksu735Node *ksu735Node = qobject_cast<Ksu735Node *>(IOServer::instance()->ksuNode())) {
        m_ksuAddressSpinBox->setValue(ksu735Node->address());
        m_ksuTimeoutSpinBox->setValue(ksu735Node->timeout());
    } else if (ModbusKsuNode *modbusKsuNode = qobject_cast<ModbusKsuNode *>(IOServer::instance()->ksuNode())) {
        m_ksuAddressSpinBox->setValue(modbusKsuNode->address());
        m_ksuTimeoutSpinBox->setValue(modbusKsuNode->timeout());
    }

    QFormLayout *addressFormLayout = new QFormLayout;
    addressFormLayout->addRow(m_ksuAddressLabel, m_ksuAddressSpinBox);

    QFormLayout *timeoutFormLayout = new QFormLayout;
    timeoutFormLayout->addRow(m_ksuTimeoutLabel, m_ksuTimeoutSpinBox);

    ui->connectionSettingsLayout->addLayout(addressFormLayout);
    ui->connectionSettingsLayout->addWidget(m_portSettings);
    ui->connectionSettingsLayout->addLayout(timeoutFormLayout);
    ui->connectionSettingsLayout->addWidget(m_savePortSettingsButton);

    connect(m_savePortSettingsButton, &QPushButton::clicked, this, &ManualTestingConnectionParametrsWidget::editSettingsClicked);
    connect(ui->noInterferenceRadioButton, &QRadioButton::clicked, this, &ManualTestingConnectionParametrsWidget::noInterferenceRadioButtonChecked);
    connect(ui->randomCountInterferenceRadioButton, &QRadioButton::clicked, this, &ManualTestingConnectionParametrsWidget::randomInterferenceRadioButtonChecked);
    connect(ui->fixedCountInterferenceRadioButton, &QRadioButton::clicked, this, &ManualTestingConnectionParametrsWidget::fixedInterferenceRadioButtonChecked);
    connect(ui->presetInterferenceRadioButton, &QRadioButton::clicked, this, &ManualTestingConnectionParametrsWidget::presetInterferenceRadioButtonChecked);
    connect(ui->randomCountInterferenceSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ManualTestingConnectionParametrsWidget::randomCountInterferenceSpinBoxChanged);
    connect(ui->fixedCountInterferenceSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ManualTestingConnectionParametrsWidget::fixedCountInterferenceSpinBoxChanged);
    connect(ui->prefixInterferenceLineEdit, &QLineEdit::textChanged, this, &ManualTestingConnectionParametrsWidget::prefixInterferenceLineEditTextChanged);
    connect(ui->postfixInterferenceLineEdit, &QLineEdit::textChanged, this, &ManualTestingConnectionParametrsWidget::postfixInterferenceLineEditTextChanged);
}

ManualTestingConnectionParametrsWidget::~ManualTestingConnectionParametrsWidget()
{
    delete ui;
}

void ManualTestingConnectionParametrsWidget::editSettingsClicked()
{
    m_portSettings->writeSettings();

    if (Ksu735Node *ksu735Node = qobject_cast<Ksu735Node *>(IOServer::instance()->ksuNode())) {
        ksu735Node->setAddress(m_ksuAddressSpinBox->value());
        ksu735Node->setTimeout(m_ksuTimeoutSpinBox->value());
    } else if (ModbusKsuNode *modbusKsuNode = qobject_cast<ModbusKsuNode *>(IOServer::instance()->ksuNode())) {
        modbusKsuNode->setAddress(m_ksuAddressSpinBox->value());
        modbusKsuNode->setTimeout(m_ksuTimeoutSpinBox->value());
    }

    emit IOServer::instance()->updatePortSettings();
}


void ManualTestingConnectionParametrsWidget::noInterferenceRadioButtonChecked()
{
    m_interferenceSender->setSenderType(InterferenceSender::NoSending);
}

void ManualTestingConnectionParametrsWidget::randomInterferenceRadioButtonChecked()
{
    m_interferenceSender->setSenderType(InterferenceSender::RandomSending);
    m_interferenceSender->setRandomSendingBytesCount(ui->randomCountInterferenceSpinBox->value());
}

void ManualTestingConnectionParametrsWidget::fixedInterferenceRadioButtonChecked()
{
    m_interferenceSender->setSenderType(InterferenceSender::FixedSending);
    m_interferenceSender->setFixedSendingBytesCount(ui->fixedCountInterferenceSpinBox->value());
}

void ManualTestingConnectionParametrsWidget::presetInterferenceRadioButtonChecked()
{
    m_interferenceSender->setSenderType(InterferenceSender::PresetSending);
    m_interferenceSender->setPrefixBytes(QByteArray::fromHex(ui->prefixInterferenceLineEdit->text().toLocal8Bit()));
    m_interferenceSender->setPostfixBytes(QByteArray::fromHex(ui->postfixInterferenceLineEdit->text().toLocal8Bit()));
}

void ManualTestingConnectionParametrsWidget::randomCountInterferenceSpinBoxChanged(int count)
{
    m_interferenceSender->setRandomSendingBytesCount(count);
}

void ManualTestingConnectionParametrsWidget::fixedCountInterferenceSpinBoxChanged(int count)
{
    m_interferenceSender->setFixedSendingBytesCount(count);
}

void ManualTestingConnectionParametrsWidget::prefixInterferenceLineEditTextChanged(const QString &text)
{
    m_interferenceSender->setPrefixBytes(QByteArray::fromHex(text.toLocal8Bit()));
}

void ManualTestingConnectionParametrsWidget::postfixInterferenceLineEditTextChanged(const QString &text)
{
    m_interferenceSender->setPostfixBytes(QByteArray::fromHex(text.toLocal8Bit()));
}
