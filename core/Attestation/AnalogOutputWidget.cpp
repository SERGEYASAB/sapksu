#include "AnalogOutputWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "AppSettings/AppSettings.h"
#include "ReportManager/OperatorClickReport.h"

static QString const SettingsGroupKey = "AttestationAnalogOutputs";
static QString const SettingsVoltageCorrectionFactorKey = "VoltageCorrectionFactor";
static QString const SettingsAmperageCorrectionFactorKey = "AmperageCorrectionFactor";

AnalogOutputWidget::AnalogOutputWidget(uint outputNumber,
                                       const QString &enableVoltageModeAlias,
                                       const QString &enableAmperageModeAlias,
                                       const QString &voltageValueAlias,
                                       const QString &amperageValueAlias,
                                       QWidget *parent) :
    QWidget(parent),
    m_outputNumber(outputNumber)
{
    auto settings = AppSettings::settings();
    settings->beginGroup(SettingsGroupKey);
    QVariant voltageCorrectionFactorVariant = settings->value(QString(SettingsVoltageCorrectionFactorKey + QString::number(m_outputNumber)),
                                                              1.0);
    QVariant amperageCorrectionFactorVariant = settings->value(QString(SettingsAmperageCorrectionFactorKey + QString::number(m_outputNumber)),
                                                               1.0);
    bool ok;
    double voltageCorrectionFactor = voltageCorrectionFactorVariant.toDouble(&ok);
    if (!ok)
        voltageCorrectionFactor = 1.0;

    double amperageCorrectionFactor = amperageCorrectionFactorVariant.toDouble(&ok);
    if (!ok)
        amperageCorrectionFactor = 1.0;

    settings->endGroup();



    QHBoxLayout *disableButtonLayot = new QHBoxLayout;

    m_disableOutputRadioButton = new QRadioButton("отключен (выходной ток 0 мА)");
    m_disableOutputRadioButton->setChecked(true);
    connect(m_disableOutputRadioButton, &QRadioButton::toggled, this, &AnalogOutputWidget::disableOutput);

    disableButtonLayot->addWidget(m_disableOutputRadioButton);
    disableButtonLayot->addStretch(1);


    QHBoxLayout *voltageLayout = new QHBoxLayout;

    m_enable0_10VRadioButton = new QRadioButton("0..10 В");
    m_enable0_10VRadioButton->setMinimumWidth(80);
    connect(m_enable0_10VRadioButton, &QRadioButton::toggled, this, &AnalogOutputWidget::enable0_10V);

    QLabel *voltageValueLabel = new QLabel("значение:");

    m_voltageSpinBox = new QDoubleSpinBox();
    m_voltageSpinBox->setMinimum(0);
    m_voltageSpinBox->setMaximum(10);
    m_voltageSpinBox->setDecimals(4);
    m_voltageSpinBox->setValue(0);
    m_voltageSpinBox->setSingleStep(0.0024420);
    m_voltageSpinBox->setSuffix(" В");
    connect(m_voltageSpinBox,   static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,               &AnalogOutputWidget::voltageOutputValueChanged);

    QLabel *voltageCorrectionFactorLabel = new QLabel("коэф. коррекции:");

    m_voltageCorrectionFactorSpinBox = new QDoubleSpinBox();
    m_voltageCorrectionFactorSpinBox->setMinimum(0.8);
    m_voltageCorrectionFactorSpinBox->setMaximum(1.2);
    m_voltageCorrectionFactorSpinBox->setDecimals(4);
    m_voltageCorrectionFactorSpinBox->setValue(voltageCorrectionFactor);
    m_voltageCorrectionFactorSpinBox->setSingleStep(0.0005);
    connect(m_voltageCorrectionFactorSpinBox,   static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,                               &AnalogOutputWidget::voltageCorrectionFactorValueChanged);

    voltageLayout->addWidget(m_enable0_10VRadioButton);
    voltageLayout->addSpacing(5);
    voltageLayout->addWidget(voltageValueLabel);
    voltageLayout->addWidget(m_voltageSpinBox);
    voltageLayout->addSpacing(5);
    voltageLayout->addWidget(voltageCorrectionFactorLabel);
    voltageLayout->addWidget(m_voltageCorrectionFactorSpinBox);


    QHBoxLayout *amperageLayout = new QHBoxLayout;

    m_enable4_20mARadioButton = new QRadioButton("0..20 мА");
    m_enable4_20mARadioButton->setMinimumWidth(80);
    connect(m_enable4_20mARadioButton, &QRadioButton::toggled, this, &AnalogOutputWidget::enable4_20mA);

    QLabel *amperageValueLabel = new QLabel("значение:");

    m_amperageSpinBox = new QDoubleSpinBox();
    m_amperageSpinBox->setMinimum(4);
    m_amperageSpinBox->setMaximum(20);
    m_amperageSpinBox->setDecimals(4);
    m_amperageSpinBox->setValue(4);
    m_amperageSpinBox->setSingleStep(0.0039070);
    m_amperageSpinBox->setSuffix(" мА");
    connect(m_amperageSpinBox,  static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,               &AnalogOutputWidget::amperageOutputValueChanged);

    QLabel *amperageCorrectionFactorLabel = new QLabel("коэф. коррекции:");

    m_amperageCorrectionFactorSpinBox = new QDoubleSpinBox();
    m_amperageCorrectionFactorSpinBox->setMinimum(0.8);
    m_amperageCorrectionFactorSpinBox->setMaximum(1.2);
    m_amperageCorrectionFactorSpinBox->setDecimals(4);
    m_amperageCorrectionFactorSpinBox->setValue(amperageCorrectionFactor);
    m_amperageCorrectionFactorSpinBox->setSingleStep(0.0005);
    connect(m_amperageCorrectionFactorSpinBox,  static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,                               &AnalogOutputWidget::amperageCorrectionFactorValueChanged);

    amperageLayout->addWidget(m_enable4_20mARadioButton);
    amperageLayout->addSpacing(5);
    amperageLayout->addWidget(amperageValueLabel);
    amperageLayout->addWidget(m_amperageSpinBox);
    amperageLayout->addSpacing(5);
    amperageLayout->addWidget(amperageCorrectionFactorLabel);
    amperageLayout->addWidget(m_amperageCorrectionFactorSpinBox);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addLayout(disableButtonLayot);
    groupBoxLayout->addLayout(voltageLayout);
    groupBoxLayout->addLayout(amperageLayout);

    QString outputName = QString("Аналоговый выход ") + QString::number(outputNumber);
    m_groupBox = new QGroupBox(outputName);
    m_groupBox->setLayout(groupBoxLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_groupBox);
    setLayout(mainLayout);

    IOServer *ioServer = IOServer::instance();
    if (ioServer) {
        m_enableVoltageModeVariable = ioServer->variableByAlias(enableVoltageModeAlias);
        m_enableAmperageModeVariable = ioServer->variableByAlias(enableAmperageModeAlias);
        m_voltageOutputValueVariable = ioServer->variableByAlias(voltageValueAlias);
        m_amperageOutputValueVariable = ioServer->variableByAlias(amperageValueAlias);
    }

    disableOutput(true);
    init = false;
}

AnalogOutputWidget::~AnalogOutputWidget()
{
    auto settings = AppSettings::settings();
    settings->beginGroup(SettingsGroupKey);
    settings->setValue(QString(SettingsVoltageCorrectionFactorKey + QString::number(m_outputNumber)),
                       m_voltageCorrectionFactorSpinBox->value());
    settings->setValue(QString(SettingsAmperageCorrectionFactorKey + QString::number(m_outputNumber)),
                       m_amperageCorrectionFactorSpinBox->value());
    settings->endGroup();
}

void AnalogOutputWidget::disableOutput(bool checked)
{
    if (!checked)
        return;

    if (!m_enableVoltageModeVariable || !m_enableAmperageModeVariable)
        return;

    m_enableVoltageModeVariable->writeValue(false);
    m_enableAmperageModeVariable->writeValue(false);

    QString message = "Аттестация: отключить аналоговый выход ";
    if (m_voltageOutputValueVariable) {
        m_voltageOutputValueVariable->writeValue(0);
        message += QString("\"%1\"").arg(m_voltageOutputValueVariable->alias());
    }

    if (m_amperageOutputValueVariable) {
        m_amperageOutputValueVariable->writeValue(4);
        message += QString("(\"%1\")").arg(m_amperageOutputValueVariable->alias());
    }

    if (!init)
        OperatorClickReport::instance()->write(message);
}

void AnalogOutputWidget::enable0_10V(bool checked)
{
    if (!checked)
        return;

    if (!m_enableVoltageModeVariable || !m_enableAmperageModeVariable)
        return;

    m_enableAmperageModeVariable->writeValue(false);
    m_enableVoltageModeVariable->writeValue(true);
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: режим аналогового выхода 0-10В"));
    setVoltage();
}

void AnalogOutputWidget::enable4_20mA(bool checked)
{
    if (!checked)
        return;

    if (!m_enableVoltageModeVariable || !m_enableAmperageModeVariable)
        return;

    m_enableVoltageModeVariable->writeValue(false);
    m_enableAmperageModeVariable->writeValue(true);
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: режим аналогового выхода 4-20мА"));
    setAmperage();
}

void AnalogOutputWidget::voltageOutputValueChanged(double value)
{
    Q_UNUSED(value)
    setVoltage();
}

void AnalogOutputWidget::amperageOutputValueChanged(double value)
{
    Q_UNUSED(value)
    setAmperage();
}

void AnalogOutputWidget::voltageCorrectionFactorValueChanged(double value)
{
    Q_UNUSED(value)
    setVoltage();
}

void AnalogOutputWidget::amperageCorrectionFactorValueChanged(double value)
{
    Q_UNUSED(value)
    setAmperage();
}

void AnalogOutputWidget::setVoltage()
{
    if (!m_enable0_10VRadioButton->isChecked())
        return;

    if (!m_voltageOutputValueVariable)
        return;

    double correctedValue = m_voltageSpinBox->value() * m_voltageCorrectionFactorSpinBox->value();

    bool ok;

    QVariant minValue = m_voltageOutputValueVariable->minValue();
    double minDoubleValue = minValue.toDouble(&ok);
    if (minValue.isValid() && !minValue.isNull() && ok) {
        if (correctedValue < minDoubleValue) {
            correctedValue = minDoubleValue;
        }
    }

    QVariant maxValue = m_voltageOutputValueVariable->maxValue();
    double maxDoubleValue = maxValue.toDouble(&ok);
    if (maxValue.isValid() && !maxValue.isNull() && ok) {
        if (correctedValue > maxDoubleValue) {
            correctedValue = maxDoubleValue;
        }
    }

    m_voltageOutputValueVariable->writeValue(correctedValue);
}

void AnalogOutputWidget::setAmperage()
{
    if (!m_enable4_20mARadioButton->isChecked())
        return;

    if (!m_amperageOutputValueVariable)
        return;

    double correctedValue = m_amperageSpinBox->value() * m_amperageCorrectionFactorSpinBox->value();

    bool ok;

    QVariant minValue = m_amperageOutputValueVariable->minValue();
    double minDoubleValue = minValue.toDouble(&ok);
    if (minValue.isValid() && !minValue.isNull() && ok) {
        if (correctedValue < minDoubleValue) {
            correctedValue = minDoubleValue;
        }
    }

    QVariant maxValue = m_amperageOutputValueVariable->maxValue();
    double maxDoubleValue = maxValue.toDouble(&ok);
    if (maxValue.isValid() && !maxValue.isNull() && ok) {
        if (correctedValue > maxDoubleValue) {
            correctedValue = maxDoubleValue;
        }
    }

    m_amperageOutputValueVariable->writeValue(correctedValue);
}
