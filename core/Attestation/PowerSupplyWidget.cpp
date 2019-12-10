#include "PowerSupplyWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "ReportManager/OperatorClickReport.h"
#include "AppSettings/AppSettings.h"

static QString const SettingsGroupKey = "AttestationPowerSupply";
static QString const SettingsKsuVoltage220ACCorrectionFactorKey = "KsuVoltage220ACCorrectionFactor";
static QString const SettingsKsuVoltage15ACCorrectionFactorKey = "KsuVoltage15ACCorrectionFactor";
static QString const SettingsKsuVoltage24DCCorrectionFactorKey = "KsuVoltage24DCCorrectionFactor";
static QString const SettingsKsuCircuit1Voltage20ACCorrectionFactorKey = "KsuCircuit1Voltage20ACCorrectionFactor";

static const double KsuVoltage220ACCoefficient = 0.5789;
static const double KsuVoltage15ACCoefficient = 9.1666;
static const double KsuVoltage24DCCoefficient = 6.933;
static const double KsuCircuit1Voltage20ACCoefficient = 1.727;

PowerSupplyWidget::PowerSupplyWidget(QWidget *parent) : QWidget(parent)
{
    setupKsuPowerSupplyOutputGroupBoxUI();
    setupKsuPowerSupplyCircuit1GroupBoxUI();
    setupKsuPowerSupplyCircuit2GroupBoxUI();
    setupTmsPowerSupplyGroupBoxUI();
    setupVariables();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_ksuPowerSupplyOutputGroupBox);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_ksuPowerSupplyCircuit1GroupBox);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_ksuPowerSupplyCircuit2GroupBox);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_tmsPowerSupplyGroupBox);
    mainLayout->addStretch(1);

    setLayout(mainLayout);
}

PowerSupplyWidget::~PowerSupplyWidget()
{
    m_ksuDisableOutputRadioButton->setChecked(true);
    m_ksuDisableCircuit1RadioButton->setChecked(true);

    auto settings = AppSettings::settings();
    settings->beginGroup(SettingsGroupKey);
    settings->setValue(SettingsKsuVoltage220ACCorrectionFactorKey, m_ksuVoltage220ACCorrectionFactorSpinBox->value());
    settings->setValue(SettingsKsuVoltage15ACCorrectionFactorKey, m_ksuVoltage15ACCorrectionFactorSpinBox->value());
    settings->setValue(SettingsKsuVoltage24DCCorrectionFactorKey, m_ksuVoltage24DCCorrectionFactorSpinBox->value());
    settings->setValue(SettingsKsuCircuit1Voltage20ACCorrectionFactorKey, m_ksuCircuit1CorrectionFactorSpinBox->value());
    settings->endGroup();
}

void PowerSupplyWidget::pvKsu220ACVoltageValueChanged()
{
    if (!m_ksuEnable220ACRadioButton->isChecked())
        return;

    if (!m_pvKsu220ACVoltageVariable)
        return;

    QVariant variableValue = m_pvKsu220ACVoltageVariable->value();
    bool ok;
    double doubleValue = variableValue.toDouble(&ok);

    if (!ok)
        return;

    QString stringValue = SapKsu::outputToPrecision(doubleValue, 2);
    m_ksuVoltage220ACMeasuredLineEdit->setText(stringValue);
}

void PowerSupplyWidget::pvKsu15ACVoltageValueChanged()
{
    if (!m_ksuEnable15ACRadioButton->isChecked())
        return;

    if (!m_pvKsu15ACVoltageVariable)
        return;

    QVariant variableValue = m_pvKsu15ACVoltageVariable->value();
    bool ok;
    double doubleValue = variableValue.toDouble(&ok);

    if (!ok)
        return;

    QString stringValue = SapKsu::outputToPrecision(doubleValue, 2);
    m_ksuVoltage15ACMeasuredLineEdit->setText(stringValue);
}

void PowerSupplyWidget::pvKsu24DCVoltageValueChanged()
{
    if (!m_ksuEnable24DCRadioButton->isChecked())
        return;

    if (!m_pvKsu24DCVoltageVariable)
        return;

    QVariant variableValue = m_pvKsu24DCVoltageVariable->value();
    bool ok;
    double doubleValue = variableValue.toDouble(&ok);

    if (!ok)
        return;

    QString stringValue = SapKsu::outputToPrecision(doubleValue, 2);
    m_ksuVoltage24DCMeasuredLineEdit->setText(stringValue);
}

void PowerSupplyWidget::pvKsuCircuit20ACVoltageValueChanged()
{
    if (!m_ksuEnableCircuit1RadioButton->isChecked())
        return;

    if (!m_pvKsuCircuit20ACVoltageVariable)
        return;

    QVariant variableValue = m_pvKsuCircuit20ACVoltageVariable->value();
    bool ok;
    double doubleValue = variableValue.toDouble(&ok);

    if (!ok)
        return;

    QString stringValue = SapKsu::outputToPrecision(doubleValue, 2);
    m_ksuCircuit1MeasuredLineEdit->setText(stringValue);
}

void PowerSupplyWidget::pv220ACVoltageValueChanged()
{
    if (m_ksuDisableCircuit2RadioButton->isChecked() && m_tmsDisableRadioButton->isChecked())
        return;

    if (!m_pv220ACVoltageVariable)
        return;

    QVariant variableValue = m_pv220ACVoltageVariable->value();
    bool ok;
    double doubleValue = variableValue.toDouble(&ok);

    if (!ok)
        return;

    QString stringValue = SapKsu::outputToPrecision(doubleValue, 2);

    if (m_ksuEnableCircuit2RadioButton->isChecked()) {
        m_ksuCircuit2MeasuredLineEdit->setText(stringValue);
    }

    if (m_tmsEnableRadioButton->isChecked()) {
        m_tmsMeasuredLineEdit->setText(stringValue);
    }
}

void PowerSupplyWidget::ksuDisableOutput(bool checked)
{
    if (!checked)
        return;

    m_ksuDisableOutputRadioButton->setChecked(true);
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: отключить выход КСУ 220В"));
}

void PowerSupplyWidget::ksuEnable220AC(bool checked)
{
    if (!checked) {
        m_pvKsu220ACVoltageVariable->disconnect(this);
        m_ksuVoltage220ACMeasuredLineEdit->clear();

        if (m_uz1StartStopVariable)
            m_uz1StartStopVariable->setValue(false);

        if (m_enableKsu220ACOutputVariable)
            m_enableKsu220ACOutputVariable->setValue(false);

        if (m_enableKsu220ACAdditionalOutputVariable)
            m_enableKsu220ACAdditionalOutputVariable->setValue(false);

        if (m_uz1WorkPermitVariable)
            m_uz1WorkPermitVariable->setValue(false);

        return;
    }

    if (!showEnablePowerQuestion("КСУ ~220 В")) {
        ksuDisableOutput(true);
        return;
    }

    if (m_pvKsu220ACVoltageVariable) {
        connect(m_pvKsu220ACVoltageVariable, &IVariable::valueChanged, this, &PowerSupplyWidget::pvKsu220ACVoltageValueChanged);
        pvKsu220ACVoltageValueChanged();
    }

    if (!m_uz1WorkPermitVariable ||
        !m_enableKsu220ACOutputVariable ||
        !m_enableKsu220ACAdditionalOutputVariable ||
        !m_uz1WriteUVariable ||
        !m_uz1StartStopVariable) {
        return;
    }

    m_uz1WorkPermitVariable->setValue(true);
    m_enableKsu220ACOutputVariable->setValue(true);
    m_enableKsu220ACAdditionalOutputVariable->setValue(true);
    setKsuVoltage220AC();
    m_uz1StartStopVariable->setValue(true);
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: включить выход питания КСУ ~220В"));
}

void PowerSupplyWidget::ksuEnable15AC(bool checked)
{
    if (!checked) {
        m_pvKsu15ACVoltageVariable->disconnect(this);
        m_ksuVoltage15ACMeasuredLineEdit->clear();

        if (m_uz1StartStopVariable)
            m_uz1StartStopVariable->setValue(false);

        if (m_enableKsu15ACOutputVariable)
            m_enableKsu15ACOutputVariable->setValue(false);

        if (m_uz1WorkPermitVariable)
            m_uz1WorkPermitVariable->setValue(false);

        return;
    }

    if (!showEnablePowerQuestion("КСУ ~15 В")) {
        ksuDisableOutput(true);
        return;
    }

    if (m_pvKsu15ACVoltageVariable) {
        connect(m_pvKsu15ACVoltageVariable, &IVariable::valueChanged, this, &PowerSupplyWidget::pvKsu15ACVoltageValueChanged);
        pvKsu15ACVoltageValueChanged();
    }

    if (!m_uz1WorkPermitVariable ||
        !m_enableKsu15ACOutputVariable ||
        !m_uz1WriteUVariable ||
        !m_uz1StartStopVariable) {
        return;
    }

    m_uz1WorkPermitVariable->setValue(true);
    m_enableKsu15ACOutputVariable->setValue(true);
    setKsuVoltage15AC();
    m_uz1StartStopVariable->setValue(true);
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: включить выход питания КСУ ~15В"));
}

void PowerSupplyWidget::ksuEnable24DC(bool checked)
{
    if (!checked) {
        m_pvKsu24DCVoltageVariable->disconnect(this);
        m_ksuVoltage24DCMeasuredLineEdit->clear();

        if (m_uz1StartStopVariable)
            m_uz1StartStopVariable->setValue(false);

        if (m_enableKsu24DCOutputVariable)
            m_enableKsu24DCOutputVariable->setValue(false);

        if (m_enableKsu24DCAdditionalOutputVariable)
            m_enableKsu24DCAdditionalOutputVariable->setValue(false);

        if (m_uz1WorkPermitVariable)
            m_uz1WorkPermitVariable->setValue(false);

        return;
    }

    if (!showEnablePowerQuestion("КСУ --24 В")) {
        ksuDisableOutput(true);
        return;
    }

    if (m_pvKsu24DCVoltageVariable) {
        connect(m_pvKsu24DCVoltageVariable, &IVariable::valueChanged, this, &PowerSupplyWidget::pvKsu24DCVoltageValueChanged);
        pvKsu24DCVoltageValueChanged();
    }

    if (!m_uz1WorkPermitVariable ||
        !m_enableKsu24DCOutputVariable ||
        !m_enableKsu24DCAdditionalOutputVariable ||
        !m_uz1WriteUVariable ||
        !m_uz1StartStopVariable) {
        return;
    }

    m_uz1WorkPermitVariable->setValue(true);
    m_enableKsu24DCOutputVariable->setValue(true);
    m_enableKsu24DCAdditionalOutputVariable->setValue(true);
    setKsuVoltage24DC();
    m_uz1StartStopVariable->setValue(true);
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: включить выход питания КСУ ~24В"));
}

void PowerSupplyWidget::ksuDisableCircuit1Output(bool checked)
{
    if (!checked)
        return;

    if (m_pvKsuCircuit20ACVoltageVariable) {
        m_pvKsuCircuit20ACVoltageVariable->disconnect(this);
    }

    m_ksuCircuit1MeasuredLineEdit->clear();

    if (m_uz2StartStopVariable)
        m_uz2StartStopVariable->setValue(false);

    if (m_enableKsuCircuit1OutputVariable)
        m_enableKsuCircuit1OutputVariable->setValue(false);

    if (m_uz2WorkPermitVariable)
        m_uz2WorkPermitVariable->setValue(false);
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: отключить выход питания цепей управления КСУ №1"));
}

void PowerSupplyWidget::ksuEnableCircuit1Output(bool checked)
{
    if (!checked)
        return;

    if (!showEnablePowerQuestion("цепей управления КСУ №1 ~20 В")) {
        ksuDisableOutput(true);
        return;
    }

    if (m_pvKsuCircuit20ACVoltageVariable) {
        connect(m_pvKsuCircuit20ACVoltageVariable,  &IVariable::valueChanged,
                this,                               &PowerSupplyWidget::pvKsuCircuit20ACVoltageValueChanged);

        pvKsuCircuit20ACVoltageValueChanged();
    }


    if (!m_uz2WorkPermitVariable || !m_enableKsuCircuit1OutputVariable || !m_uz2StartStopVariable || !m_uz2WriteUVariable)
        return;

    m_uz2WorkPermitVariable->setValue(true);
    m_enableKsuCircuit1OutputVariable->setValue(true);
    setKsuCircuit1Voltage20AC();
    m_uz2StartStopVariable->setValue(true);
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: включить выход питания цепей управления КСУ №1"));
}

void PowerSupplyWidget::ksuDisableCircuit2Output(bool checked)
{
    if (!checked)
        return;

    m_ksuCircuit2MeasuredLineEdit->clear();

    if (m_enableKsuCircuit2OutputVariable) {
        m_enableKsuCircuit2OutputVariable->writeValue(false);
        OperatorClickReport::instance()->write(QStringLiteral("Аттестация: отключить выход питания цепей управления КСУ №2"));
    }
}

void PowerSupplyWidget::ksuEnableCircuit2Output(bool checked)
{
    if (!checked)
        return;

    if (!showEnablePowerQuestion("цепей управления КСУ №2 ~220 В")) {
        ksuDisableOutput(true);
        return;
    }

    if (m_enableKsuCircuit2OutputVariable) {
        m_enableKsuCircuit2OutputVariable->writeValue(true);
        OperatorClickReport::instance()->write(QStringLiteral("Аттестация: включить выход питания цепей управления КСУ №2"));
    }

    pv220ACVoltageValueChanged();
}

void PowerSupplyWidget::tmsDisableOutput(bool checked)
{
    if (!checked)
        return;

    m_tmsMeasuredLineEdit->clear();

    if (m_enableTmsOutputVariable) {
        m_enableTmsOutputVariable->writeValue(false);
        OperatorClickReport::instance()->write(QStringLiteral("Аттестация: отключить выход питания ТМС"));
    }
}

void PowerSupplyWidget::tmsEnableOutput(bool checked)
{
    if (!checked)
        return;

    if (!showEnablePowerQuestion("ТМС ~220 В")) {
        ksuDisableOutput(true);
        return;
    }

    if (m_enableTmsOutputVariable) {
        m_enableTmsOutputVariable->writeValue(true);
        OperatorClickReport::instance()->write(QStringLiteral("Аттестация: включить выход питания ТМС"));
    }

    pv220ACVoltageValueChanged();
}

void PowerSupplyWidget::ksuVoltage220ACPredeterminedSpinBoxValueChanged(int value)
{
    Q_UNUSED(value)
    setKsuVoltage220AC();
}

void PowerSupplyWidget::ksuVoltage15ACPredeterminedSpinBoxValueChanged(int value)
{
    Q_UNUSED(value)
    setKsuVoltage15AC();
}

void PowerSupplyWidget::ksuVoltage24DCPredeterminedSpinBoxValueChanged(int value)
{
    Q_UNUSED(value)
    setKsuVoltage24DC();
}

void PowerSupplyWidget::ksuCircuit1PredeterminedSpinBoxValueChanged(int value)
{
    Q_UNUSED(value)
    setKsuCircuit1Voltage20AC();
}

void PowerSupplyWidget::ksuVoltage220ACCorrectionFactorSpinBoxValueChanged(double value)
{
    Q_UNUSED(value)
    setKsuVoltage220AC();
}

void PowerSupplyWidget::ksuVoltage15ACCorrectionFactorSpinBoxValueChanged(double value)
{
    Q_UNUSED(value)
    setKsuVoltage15AC();
}

void PowerSupplyWidget::ksuVoltage24DCCorrectionFactorSpinBoxValueChanged(double value)
{
    Q_UNUSED(value)
    setKsuVoltage24DC();
}

void PowerSupplyWidget::ksuCircuit1CorrectionFactorSpinBoxValueChanged(double value)
{
    Q_UNUSED(value)
    setKsuCircuit1Voltage20AC();
}

void PowerSupplyWidget::setKsuVoltage220AC()
{
    if (!m_ksuEnable220ACRadioButton->isChecked())
        return;

    double calculatedU = KsuVoltage220ACCoefficient *
                         m_ksuVoltage220ACPredeterminedSpinBox->value() *
                         m_ksuVoltage220ACCorrectionFactorSpinBox->value();

    double writedU = qFloor(calculatedU);

    qDebug() << "setKsuVoltage220AC" << writedU;

    if (m_uz1WriteUVariable)
        m_uz1WriteUVariable->setValue(writedU);
}

void PowerSupplyWidget::setKsuVoltage15AC()
{
    if (!m_ksuEnable15ACRadioButton->isChecked())
        return;

    double calculatedU = KsuVoltage15ACCoefficient *
                         m_ksuVoltage15ACPredeterminedSpinBox->value() *
                         m_ksuVoltage15ACCorrectionFactorSpinBox->value();

    double writedU = qFloor(calculatedU);

    qDebug() << "setKsuVoltage15AC" << writedU;

    if (m_uz1WriteUVariable)
        m_uz1WriteUVariable->setValue(writedU);
}

void PowerSupplyWidget::setKsuVoltage24DC()
{
    if (!m_ksuEnable24DCRadioButton->isChecked())
        return;

    double calculatedU = KsuVoltage24DCCoefficient *
                         m_ksuVoltage24DCPredeterminedSpinBox->value() *
                         m_ksuVoltage24DCCorrectionFactorSpinBox->value();

    double writedU = qFloor(calculatedU);

    qDebug() << "setKsuVoltage24DC" << writedU;

    if (m_uz1WriteUVariable)
        m_uz1WriteUVariable->setValue(writedU);
}

void PowerSupplyWidget::setKsuCircuit1Voltage20AC()
{
    if (!m_ksuEnableCircuit1RadioButton->isChecked())
        return;

    double calculatedU = KsuCircuit1Voltage20ACCoefficient *
                         m_ksuCircuit1PredeterminedSpinBox->value() *
                         m_ksuCircuit1CorrectionFactorSpinBox->value();

    double writedU = qFloor(calculatedU);

    qDebug() << "setKsuCircuit1Voltage20AC" << writedU;

    if (m_uz2WriteUVariable)
        m_uz2WriteUVariable->setValue(writedU);
}

void PowerSupplyWidget::setupKsuPowerSupplyOutputGroupBoxUI()
{
    auto settings = AppSettings::settings();
    settings->beginGroup(SettingsGroupKey);

    QVariant ksuVoltage220ACCorrectionFactorVariant = settings->value(SettingsKsuVoltage220ACCorrectionFactorKey, 1.0);
    QVariant ksuVoltage15ACCorrectionFactorVariant = settings->value(SettingsKsuVoltage15ACCorrectionFactorKey, 1.0);
    QVariant ksuVoltage24DCCorrectionFactorVariant = settings->value(SettingsKsuVoltage24DCCorrectionFactorKey, 1.0);

    bool ok;
    double ksuVoltage220ACCorrectionFactor = ksuVoltage220ACCorrectionFactorVariant.toDouble(&ok);
    if (!ok)
        ksuVoltage220ACCorrectionFactor = 1.0;

    double ksuVoltage15ACCorrectionFactor = ksuVoltage15ACCorrectionFactorVariant.toDouble(&ok);
    if (!ok)
        ksuVoltage15ACCorrectionFactor = 1.0;

    double ksuVoltage24DCCorrectionFactor = ksuVoltage24DCCorrectionFactorVariant.toDouble(&ok);
    if (!ok)
        ksuVoltage24DCCorrectionFactor = 1.0;

    settings->endGroup();

    ///---- 220 AC ---///
    m_ksuDisableOutputRadioButton = new QRadioButton("отключен");
    m_ksuDisableOutputRadioButton->setChecked(true);
    connect(m_ksuDisableOutputRadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::ksuDisableOutput);

    QHBoxLayout *disableButtonLayot = new QHBoxLayout;
    disableButtonLayot->addWidget(m_ksuDisableOutputRadioButton);
    disableButtonLayot->addStretch(1);


    m_ksuEnable220ACRadioButton = new QRadioButton("выход Uн = ~220 В");
    m_ksuEnable220ACRadioButton->setMinimumWidth(150);
    connect(m_ksuEnable220ACRadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::ksuEnable220AC);

    QLabel *voltage220ACValueLabel = new QLabel("значение:");
    QLabel *voltage220ACMeasuredValueLabel = new QLabel("измерение:");
    QLabel *voltage220ACMeasureUnitLabel = new QLabel("В");
    QLabel *voltage220ACCorrectionFactorLabel = new QLabel("коэф. коррекции:");

    m_ksuVoltage220ACPredeterminedSpinBox = new QSpinBox();
    m_ksuVoltage220ACPredeterminedSpinBox->setMinimum(66);
    m_ksuVoltage220ACPredeterminedSpinBox->setMaximum(286);
    m_ksuVoltage220ACPredeterminedSpinBox->setValue(220);
    m_ksuVoltage220ACPredeterminedSpinBox->setSingleStep(1);
    m_ksuVoltage220ACPredeterminedSpinBox->setSuffix(" В");
    connect(m_ksuVoltage220ACPredeterminedSpinBox,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,                                   &PowerSupplyWidget::ksuVoltage220ACPredeterminedSpinBoxValueChanged);

    m_ksuVoltage220ACMeasuredLineEdit = new QLineEdit();
    m_ksuVoltage220ACMeasuredLineEdit->setEnabled(false);

    m_ksuVoltage220ACCorrectionFactorSpinBox = new QDoubleSpinBox();
    m_ksuVoltage220ACCorrectionFactorSpinBox->setMinimum(0.5);
    m_ksuVoltage220ACCorrectionFactorSpinBox->setMaximum(1.050);
    m_ksuVoltage220ACCorrectionFactorSpinBox->setDecimals(3);
    m_ksuVoltage220ACCorrectionFactorSpinBox->setValue(ksuVoltage220ACCorrectionFactor);
    m_ksuVoltage220ACCorrectionFactorSpinBox->setSingleStep(0.005);
    connect(m_ksuVoltage220ACCorrectionFactorSpinBox,   static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,                                       &PowerSupplyWidget::ksuVoltage220ACCorrectionFactorSpinBoxValueChanged);


    QHBoxLayout *enable220ACLayout = new QHBoxLayout;
    enable220ACLayout->addWidget(m_ksuEnable220ACRadioButton);
    enable220ACLayout->addSpacing(10);
    enable220ACLayout->addWidget(voltage220ACValueLabel);
    enable220ACLayout->addWidget(m_ksuVoltage220ACPredeterminedSpinBox);
    enable220ACLayout->addSpacing(10);
    enable220ACLayout->addWidget(voltage220ACMeasuredValueLabel);
    enable220ACLayout->addWidget(m_ksuVoltage220ACMeasuredLineEdit);
    enable220ACLayout->addWidget(voltage220ACMeasureUnitLabel);
    enable220ACLayout->addSpacing(10);
    enable220ACLayout->addWidget(voltage220ACCorrectionFactorLabel);
    enable220ACLayout->addWidget(m_ksuVoltage220ACCorrectionFactorSpinBox);
    enable220ACLayout->addStretch(1);


    ///---- 20 AC ---///
    m_ksuEnable15ACRadioButton = new QRadioButton("выход Uн = ~15 В");
    m_ksuEnable15ACRadioButton->setMinimumWidth(150);
    connect(m_ksuEnable15ACRadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::ksuEnable15AC);

    QLabel *voltage15ACValueLabel = new QLabel("значение:");
    QLabel *voltage15ACMeasuredValueLabel = new QLabel("измерение:");
    QLabel *voltage15ACMeasureUnitLabel = new QLabel("В");
    QLabel *voltage15ACCorrectionFactorLabel = new QLabel("коэф. коррекции:");

    m_ksuVoltage15ACPredeterminedSpinBox = new QSpinBox();
    m_ksuVoltage15ACPredeterminedSpinBox->setMinimum(5);
    m_ksuVoltage15ACPredeterminedSpinBox->setMaximum(20);
    m_ksuVoltage15ACPredeterminedSpinBox->setValue(15);
    m_ksuVoltage15ACPredeterminedSpinBox->setSingleStep(1);
    m_ksuVoltage15ACPredeterminedSpinBox->setSuffix(" В");
    connect(m_ksuVoltage15ACPredeterminedSpinBox,   static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,                                   &PowerSupplyWidget::ksuVoltage15ACPredeterminedSpinBoxValueChanged);

    m_ksuVoltage15ACMeasuredLineEdit = new QLineEdit();
    m_ksuVoltage15ACMeasuredLineEdit->setEnabled(false);

    m_ksuVoltage15ACCorrectionFactorSpinBox = new QDoubleSpinBox();
    m_ksuVoltage15ACCorrectionFactorSpinBox->setMinimum(0.5);
    m_ksuVoltage15ACCorrectionFactorSpinBox->setMaximum(1.2);
    m_ksuVoltage15ACCorrectionFactorSpinBox->setDecimals(2);
    m_ksuVoltage15ACCorrectionFactorSpinBox->setValue(ksuVoltage15ACCorrectionFactor);
    m_ksuVoltage15ACCorrectionFactorSpinBox->setSingleStep(0.01);
    connect(m_ksuVoltage15ACCorrectionFactorSpinBox,    static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,                                       &PowerSupplyWidget::ksuVoltage15ACCorrectionFactorSpinBoxValueChanged);

    QHBoxLayout *enable20ACLayout = new QHBoxLayout;
    enable20ACLayout->addWidget(m_ksuEnable15ACRadioButton);
    enable20ACLayout->addSpacing(10);
    enable20ACLayout->addWidget(voltage15ACValueLabel);
    enable20ACLayout->addWidget(m_ksuVoltage15ACPredeterminedSpinBox);
    enable20ACLayout->addSpacing(10);
    enable20ACLayout->addWidget(voltage15ACMeasuredValueLabel);
    enable20ACLayout->addWidget(m_ksuVoltage15ACMeasuredLineEdit);
    enable20ACLayout->addWidget(voltage15ACMeasureUnitLabel);
    enable20ACLayout->addSpacing(10);
    enable20ACLayout->addWidget(voltage15ACCorrectionFactorLabel);
    enable20ACLayout->addWidget(m_ksuVoltage15ACCorrectionFactorSpinBox);
    enable20ACLayout->addStretch(1);


    ///---- 24 DC ---///;
    m_ksuEnable24DCRadioButton = new QRadioButton("выход Uн = --24 В");
    m_ksuEnable24DCRadioButton->setMinimumWidth(150);
    connect(m_ksuEnable24DCRadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::ksuEnable24DC);

    QLabel *voltage24DCValueLabel = new QLabel("значение:");
    QLabel *voltage24DCMeasuredValueLabel = new QLabel("измерение:");
    QLabel *voltage24DCMeasureUnitLabel = new QLabel("В");
    QLabel *voltage24DCCorrectionFactorLabel = new QLabel("коэф. коррекции:");

    m_ksuVoltage24DCPredeterminedSpinBox = new QSpinBox();
    m_ksuVoltage24DCPredeterminedSpinBox->setMinimum(20);
    m_ksuVoltage24DCPredeterminedSpinBox->setMaximum(25);
    m_ksuVoltage24DCPredeterminedSpinBox->setValue(24);
    m_ksuVoltage24DCPredeterminedSpinBox->setSingleStep(1);
    m_ksuVoltage24DCPredeterminedSpinBox->setSuffix(" В");
    connect(m_ksuVoltage24DCPredeterminedSpinBox,   static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,                                   &PowerSupplyWidget::ksuVoltage24DCPredeterminedSpinBoxValueChanged);

    m_ksuVoltage24DCMeasuredLineEdit = new QLineEdit();
    m_ksuVoltage24DCMeasuredLineEdit->setEnabled(false);

    m_ksuVoltage24DCCorrectionFactorSpinBox = new QDoubleSpinBox();
    m_ksuVoltage24DCCorrectionFactorSpinBox->setMinimum(0.5);
    m_ksuVoltage24DCCorrectionFactorSpinBox->setMaximum(1.2);
    m_ksuVoltage24DCCorrectionFactorSpinBox->setDecimals(2);
    m_ksuVoltage24DCCorrectionFactorSpinBox->setValue(ksuVoltage24DCCorrectionFactor);
    m_ksuVoltage24DCCorrectionFactorSpinBox->setSingleStep(0.01);
    connect(m_ksuVoltage24DCCorrectionFactorSpinBox,    static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,                                       &PowerSupplyWidget::ksuVoltage24DCCorrectionFactorSpinBoxValueChanged);

    QHBoxLayout *enable24DCLayout = new QHBoxLayout;
    enable24DCLayout->addWidget(m_ksuEnable24DCRadioButton);
    enable24DCLayout->addSpacing(10);
    enable24DCLayout->addWidget(voltage24DCValueLabel);
    enable24DCLayout->addWidget(m_ksuVoltage24DCPredeterminedSpinBox);
    enable24DCLayout->addSpacing(10);
    enable24DCLayout->addWidget(voltage24DCMeasuredValueLabel);
    enable24DCLayout->addWidget(m_ksuVoltage24DCMeasuredLineEdit);
    enable24DCLayout->addWidget(voltage24DCMeasureUnitLabel);
    enable24DCLayout->addSpacing(10);
    enable24DCLayout->addWidget(voltage24DCCorrectionFactorLabel);
    enable24DCLayout->addWidget(m_ksuVoltage24DCCorrectionFactorSpinBox);
    enable24DCLayout->addStretch(1);


    ///--- ksu power supply widget ---///
    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBoxLayout->addLayout(disableButtonLayot);
    groupBoxLayout->addLayout(enable220ACLayout);
    groupBoxLayout->addLayout(enable20ACLayout);
    groupBoxLayout->addLayout(enable24DCLayout);


    m_ksuPowerSupplyOutputGroupBox = new QGroupBox("Выход питания КСУ");
    m_ksuPowerSupplyOutputGroupBox->setLayout(groupBoxLayout);
}

void PowerSupplyWidget::setupKsuPowerSupplyCircuit1GroupBoxUI()
{
    auto settings = AppSettings::settings();
    settings->beginGroup(SettingsGroupKey);

    QVariant ksuCircuit1Voltage20ACCorrectionFactorVariant = settings->value(SettingsKsuCircuit1Voltage20ACCorrectionFactorKey, 1.0);

    bool ok;
    double ksuCircuit1Voltage20ACCorrectionFactor = ksuCircuit1Voltage20ACCorrectionFactorVariant.toDouble(&ok);
    if (!ok)
        ksuCircuit1Voltage20ACCorrectionFactor = 1.0;

    settings->endGroup();



    m_ksuDisableCircuit1RadioButton = new QRadioButton("отключен");
    m_ksuDisableCircuit1RadioButton->setChecked(true);
    connect(m_ksuDisableCircuit1RadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::ksuDisableCircuit1Output);

    QHBoxLayout *disableButtonLayot = new QHBoxLayout;
    disableButtonLayot->addWidget(m_ksuDisableCircuit1RadioButton);
    disableButtonLayot->addStretch(1);


    m_ksuEnableCircuit1RadioButton = new QRadioButton("выход Uн = ~20 В");
    m_ksuEnableCircuit1RadioButton->setMinimumWidth(150);
    connect(m_ksuEnableCircuit1RadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::ksuEnableCircuit1Output);

    QLabel *voltageValueLabel = new QLabel("значение:");
    QLabel *voltageMeasuredValueLabel = new QLabel("измерение:");
    QLabel *voltageMeasureUnitLabel = new QLabel("В");
    QLabel *voltageCorrectionFactorLabel = new QLabel("коэф. коррекции:");

    m_ksuCircuit1PredeterminedSpinBox = new QSpinBox();
    m_ksuCircuit1PredeterminedSpinBox->setMinimum(10);
    m_ksuCircuit1PredeterminedSpinBox->setMaximum(30);
    m_ksuCircuit1PredeterminedSpinBox->setValue(20);
    m_ksuCircuit1PredeterminedSpinBox->setSingleStep(1);
    m_ksuCircuit1PredeterminedSpinBox->setSuffix(" В");
    connect(m_ksuCircuit1PredeterminedSpinBox,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,                               &PowerSupplyWidget::ksuCircuit1PredeterminedSpinBoxValueChanged);

    m_ksuCircuit1MeasuredLineEdit = new QLineEdit();
    m_ksuCircuit1MeasuredLineEdit->setEnabled(false);

    m_ksuCircuit1CorrectionFactorSpinBox = new QDoubleSpinBox();
    m_ksuCircuit1CorrectionFactorSpinBox->setMinimum(0.1);
    m_ksuCircuit1CorrectionFactorSpinBox->setMaximum(3);
    m_ksuCircuit1CorrectionFactorSpinBox->setDecimals(2);
    m_ksuCircuit1CorrectionFactorSpinBox->setValue(ksuCircuit1Voltage20ACCorrectionFactor);
    m_ksuCircuit1CorrectionFactorSpinBox->setSingleStep(0.01);
    connect(m_ksuCircuit1CorrectionFactorSpinBox,   static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,                                   &PowerSupplyWidget::ksuCircuit1CorrectionFactorSpinBoxValueChanged);

    QHBoxLayout *enableButtonLayout = new QHBoxLayout;
    enableButtonLayout->addWidget(m_ksuEnableCircuit1RadioButton);
    enableButtonLayout->addSpacing(10);
    enableButtonLayout->addWidget(voltageValueLabel);
    enableButtonLayout->addWidget(m_ksuCircuit1PredeterminedSpinBox);
    enableButtonLayout->addSpacing(10);
    enableButtonLayout->addWidget(voltageMeasuredValueLabel);
    enableButtonLayout->addWidget(m_ksuCircuit1MeasuredLineEdit);
    enableButtonLayout->addWidget(voltageMeasureUnitLabel);
    enableButtonLayout->addSpacing(10);
    enableButtonLayout->addWidget(voltageCorrectionFactorLabel);
    enableButtonLayout->addWidget(m_ksuCircuit1CorrectionFactorSpinBox);
    enableButtonLayout->addStretch(1);


    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBoxLayout->addLayout(disableButtonLayot);
    groupBoxLayout->addLayout(enableButtonLayout);

    m_ksuPowerSupplyCircuit1GroupBox = new QGroupBox("Выход питания цепей управления КСУ №1");
    m_ksuPowerSupplyCircuit1GroupBox->setLayout(groupBoxLayout);
}

void PowerSupplyWidget::setupKsuPowerSupplyCircuit2GroupBoxUI()
{
    m_ksuDisableCircuit2RadioButton = new QRadioButton("отключен");
    m_ksuDisableCircuit2RadioButton->setChecked(true);
    connect(m_ksuDisableCircuit2RadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::ksuDisableCircuit2Output);

    QHBoxLayout *disableButtonLayot = new QHBoxLayout;
    disableButtonLayot->addWidget(m_ksuDisableCircuit2RadioButton);
    disableButtonLayot->addStretch(1);

    m_ksuEnableCircuit2RadioButton = new QRadioButton("включен Uн = ~220 В");
    m_ksuEnableCircuit2RadioButton->setMinimumWidth(150);
    connect(m_ksuEnableCircuit2RadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::ksuEnableCircuit2Output);

    QLabel *voltageMeasuredValueLabel = new QLabel("измерение:");
    QLabel *voltageMeasureUnitLabel = new QLabel("В");

    m_ksuCircuit2MeasuredLineEdit = new QLineEdit();
    m_ksuCircuit2MeasuredLineEdit->setEnabled(false);

    QHBoxLayout *enableButtonLayout = new QHBoxLayout;
    enableButtonLayout->addWidget(m_ksuEnableCircuit2RadioButton);
    enableButtonLayout->addSpacing(10);
    enableButtonLayout->addWidget(voltageMeasuredValueLabel);
    enableButtonLayout->addWidget(m_ksuCircuit2MeasuredLineEdit);
    enableButtonLayout->addWidget(voltageMeasureUnitLabel);
    enableButtonLayout->addStretch(1);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBoxLayout->addLayout(disableButtonLayot);
    groupBoxLayout->addLayout(enableButtonLayout);

    m_ksuPowerSupplyCircuit2GroupBox = new QGroupBox("Выход питания цепей управления КСУ №2");
    m_ksuPowerSupplyCircuit2GroupBox->setLayout(groupBoxLayout);
}

void PowerSupplyWidget::setupTmsPowerSupplyGroupBoxUI()
{
    m_tmsDisableRadioButton = new QRadioButton("отключен");
    m_tmsDisableRadioButton->setChecked(true);
    connect(m_tmsDisableRadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::tmsDisableOutput);

    QHBoxLayout *disableButtonLayot = new QHBoxLayout;
    disableButtonLayot->addWidget(m_tmsDisableRadioButton);
    disableButtonLayot->addStretch(1);

    m_tmsEnableRadioButton = new QRadioButton("включен Uн = ~220 В");
    m_tmsEnableRadioButton->setMinimumWidth(150);
    connect(m_tmsEnableRadioButton, &QRadioButton::toggled, this, &PowerSupplyWidget::tmsEnableOutput);

    QLabel *voltageMeasuredValueLabel = new QLabel("измерение:");
    QLabel *voltageMeasureUnitLabel = new QLabel("В");

    m_tmsMeasuredLineEdit = new QLineEdit();
    m_tmsMeasuredLineEdit->setEnabled(false);

    QHBoxLayout *enableButtonLayout = new QHBoxLayout;
    enableButtonLayout->addWidget(m_tmsEnableRadioButton);
    enableButtonLayout->addSpacing(10);
    enableButtonLayout->addWidget(voltageMeasuredValueLabel);
    enableButtonLayout->addWidget(m_tmsMeasuredLineEdit);
    enableButtonLayout->addWidget(voltageMeasureUnitLabel);
    enableButtonLayout->addStretch(1);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBoxLayout->addLayout(disableButtonLayot);
    groupBoxLayout->addLayout(enableButtonLayout);

    m_tmsPowerSupplyGroupBox = new QGroupBox("Выход питания ТМС");
    m_tmsPowerSupplyGroupBox->setLayout(groupBoxLayout);
}

void PowerSupplyWidget::setupVariables()
{
    IOServer *ioServer = IOServer::instance();
    if (!ioServer)
        return;

    m_pvKsu220ACVoltageVariable = ioServer->variableByAlias("$PV3_U");
    m_pvKsu15ACVoltageVariable = ioServer->variableByAlias("$PV2_U");
    m_pvKsu24DCVoltageVariable = ioServer->variableByAlias("$PV6_U");
    m_pvKsuCircuit20ACVoltageVariable = ioServer->variableByAlias("$PV4_U");
    m_pv220ACVoltageVariable = ioServer->variableByAlias("$PV1_U");

    m_uz1WorkPermitVariable = ioServer->variableByAlias("$A15_A0");
    m_uz2WorkPermitVariable = ioServer->variableByAlias("$A15_A1");

    m_enableKsu220ACOutputVariable = ioServer->variableByAlias("$A15_A3");
    m_enableKsu15ACOutputVariable = ioServer->variableByAlias("$A15_A4");
    m_enableKsu24DCOutputVariable = ioServer->variableByAlias("$A15_A5");

    m_enableKsu220ACAdditionalOutputVariable = ioServer->variableByAlias("$A29_B6");
    m_enableKsu24DCAdditionalOutputVariable = ioServer->variableByAlias("$A29_B5");

    m_enableKsuCircuit1OutputVariable = ioServer->variableByAlias("$A15_A6");
    m_enableKsuCircuit2OutputVariable = ioServer->variableByAlias("$A15_A7");
    m_enableTmsOutputVariable = ioServer->variableByAlias("$SAP_TMS_REAL_POWER");


    if (m_pv220ACVoltageVariable) {
        connect(m_pv220ACVoltageVariable, &IVariable::valueChanged, this, &PowerSupplyWidget::pv220ACVoltageValueChanged);
    }

    m_uz1WriteUVariable = ioServer->variableByAlias("$UZ1_WRITE_U");
    m_uz1WriteFreqVariable = ioServer->variableByAlias("$UZ1_WRITE_FREQ");
    m_uz1StartStopVariable = ioServer->variableByAlias("$UZ1_CTRL_DRIVE");

    if (m_uz1WriteFreqVariable)
        m_uz1WriteFreqVariable->writeValue(50);

    m_uz2WriteUVariable = ioServer->variableByAlias("$UZ2_WRITE_U");
    m_uz2WriteFreqVariable = ioServer->variableByAlias("$UZ2_WRITE_FREQ");
    m_uz2StartStopVariable = ioServer->variableByAlias("$UZ2_CTRL_DRIVE");

    if (m_uz2WriteFreqVariable)
        m_uz2WriteFreqVariable->writeValue(50);
}

bool PowerSupplyWidget::showEnablePowerQuestion(const QString &outputName)
{
    QString msgBoxText = QString("Вы действительно хотите включить выход питания %1?\r\n"
                                 "При нажатии кнопки \"Нет\" данный выход питания будет выключен").arg(outputName);

    QMessageBox msgBox;
    msgBox.setWindowTitle("Включение выхода питания");
    msgBox.setText(msgBoxText);
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::AcceptRole);
    QPushButton *noButton = msgBox.addButton(tr("&Нет"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {
        OperatorClickReport::instance()->write(QString("Включить выход питания %1? Нажата клавиша ДА").arg(outputName));
    } else {
        OperatorClickReport::instance()->write(QString("Включить выход питания %1? Нажата клавиша НЕТ").arg(outputName));
    }

    return msgBox.clickedButton() == yesButton;
}
