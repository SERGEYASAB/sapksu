#include "AnalogVoltageOutputWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "AppSettings/AppSettings.h"

static QString const SettingsGroupKey = "AttestationAnalogOutputs";
static QString const SettingsVoltageCorrectionFactorKey = "VoltageCorrectionFactor";

AnalogVoltageOutputWidget::AnalogVoltageOutputWidget(uint outputNumber,
                                                     const QString &voltageValueAlias,
                                                     QWidget *parent) :
    QWidget(parent),
    m_outputNumber(outputNumber)
{
    auto settings = AppSettings::settings();
    settings->beginGroup(SettingsGroupKey);
    QVariant voltageCorrectionFactorVariant = settings->value(QString(SettingsVoltageCorrectionFactorKey + QString::number(m_outputNumber)),
                                                              1.0);
    bool ok;
    double voltageCorrectionFactor = voltageCorrectionFactorVariant.toDouble(&ok);
    if (!ok)
        voltageCorrectionFactor = 1.0;

    settings->endGroup();

    QHBoxLayout *voltageLayout = new QHBoxLayout;

    m_enable0_10VRadioButton = new QRadioButton("0..10 В");
    m_enable0_10VRadioButton->setChecked(true);
    m_enable0_10VRadioButton->setMinimumWidth(80);
    m_enable0_10VRadioButton->setEnabled(false);

    QLabel *voltageValueLabel = new QLabel("значение:");

    m_voltageSpinBox = new QDoubleSpinBox();
    m_voltageSpinBox->setMinimum(0);
    m_voltageSpinBox->setMaximum(10);
    m_voltageSpinBox->setDecimals(4);
    m_voltageSpinBox->setValue(0);
    m_voltageSpinBox->setSingleStep(0.0024420);
    m_voltageSpinBox->setSuffix(" В");
    connect(m_voltageSpinBox,   static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,               &AnalogVoltageOutputWidget::voltageOutputValueChanged);

    QLabel *voltageCorrectionFactorLabel = new QLabel("коэф. коррекции:");

    m_voltageCorrectionFactorSpinBox = new QDoubleSpinBox();
    m_voltageCorrectionFactorSpinBox->setMinimum(0.8);
    m_voltageCorrectionFactorSpinBox->setMaximum(1.2);
    m_voltageCorrectionFactorSpinBox->setDecimals(4);
    m_voltageCorrectionFactorSpinBox->setValue(voltageCorrectionFactor);
    m_voltageCorrectionFactorSpinBox->setSingleStep(0.0005);
    connect(m_voltageCorrectionFactorSpinBox,   static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,                               &AnalogVoltageOutputWidget::voltageCorrectionFactorValueChanged);

    voltageLayout->addWidget(m_enable0_10VRadioButton);
    voltageLayout->addSpacing(5);
    voltageLayout->addWidget(voltageValueLabel);
    voltageLayout->addWidget(m_voltageSpinBox);
    voltageLayout->addSpacing(5);
    voltageLayout->addWidget(voltageCorrectionFactorLabel);
    voltageLayout->addWidget(m_voltageCorrectionFactorSpinBox);


    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addLayout(voltageLayout);

    QString outputName = QString("Аналоговый выход ") + QString::number(outputNumber) + QString(" (сопротивление изоляции КСУ)");
    m_groupBox = new QGroupBox(outputName);
    m_groupBox->setLayout(groupBoxLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_groupBox);
    setLayout(mainLayout);

    IOServer *ioServer = IOServer::instance();
    if (ioServer) {
        m_voltageOutputValueVariable = ioServer->variableByAlias(voltageValueAlias);
    }

    setVoltage();
}

AnalogVoltageOutputWidget::~AnalogVoltageOutputWidget()
{
    auto settings = AppSettings::settings();
    settings->beginGroup(SettingsGroupKey);
    settings->setValue(QString(SettingsVoltageCorrectionFactorKey + QString::number(m_outputNumber)),
                       m_voltageCorrectionFactorSpinBox->value());
    settings->endGroup();
}

void AnalogVoltageOutputWidget::voltageOutputValueChanged(double value)
{
    Q_UNUSED(value)
    setVoltage();
}

void AnalogVoltageOutputWidget::voltageCorrectionFactorValueChanged(double value)
{
    Q_UNUSED(value)
    setVoltage();
}

void AnalogVoltageOutputWidget::setVoltage()
{
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
