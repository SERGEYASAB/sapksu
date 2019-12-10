#include "AnalogInputWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/NumberVariableValueStrategy.h"
#include "AppSettings/AppSettings.h"
#include "ReportManager/OperatorClickReport.h"

static QString const SettingsGroupKey = "AttestationAnalogInputs";
static QString const SettingsVoltageCorrectionFactorKey = "VoltageCorrectionFactor";
static QString const SettingsAmperageCorrectionFactorKey = "AmperageCorrectionFactor";

AnalogInputWidget::AnalogInputWidget(uint inputNumber,
                                     const QString &disableInputAlias,
                                     const QString &recalculationAlias,
                                     const QString &inputValueAlias,
                                     QWidget *parent) :
    QWidget(parent),
    m_inputNumber(inputNumber)
{
    auto settings = AppSettings::settings();
    settings->beginGroup(SettingsGroupKey);
    QVariant voltageCorrectionFactorVariant = settings->value(QString(SettingsVoltageCorrectionFactorKey + QString::number(m_inputNumber)),
                                                              1.0);
    QVariant amperageCorrectionFactorVariant = settings->value(QString(SettingsAmperageCorrectionFactorKey + QString::number(m_inputNumber)),
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

    disableOutputRadioButton = new QRadioButton("отключен (закорочен на 0 В)");
    disableOutputRadioButton->setChecked(true);
    connect(disableOutputRadioButton, &QRadioButton::toggled, this, &AnalogInputWidget::disableInput);

    disableButtonLayot->addWidget(disableOutputRadioButton);
    disableButtonLayot->addStretch(1);


    QHBoxLayout *voltageLayout = new QHBoxLayout;

    m_enable0_10VRadioButton = new QRadioButton("0..10 В");
    m_enable0_10VRadioButton->setMinimumWidth(80);
    connect(m_enable0_10VRadioButton, &QRadioButton::toggled, this, &AnalogInputWidget::enable0_10V);

    QLabel *voltageValueLabel = new QLabel("значение:");
    QLabel *voltageMeasureUnitLabel = new QLabel(QStringLiteral("В"));
    voltageMeasureUnitLabel->setMinimumWidth(40);
    QLabel *voltageCorrectionFactorLabel = new QLabel("коэф. коррекции:");

    m_voltageLineEdit = new QLineEdit();
    m_voltageLineEdit->setEnabled(false);

    m_voltageCorrectionFactorSpinBox = new QDoubleSpinBox();
    m_voltageCorrectionFactorSpinBox->setMinimum(0.8);
    m_voltageCorrectionFactorSpinBox->setMaximum(1.2);
    m_voltageCorrectionFactorSpinBox->setDecimals(4);
    m_voltageCorrectionFactorSpinBox->setValue(voltageCorrectionFactor);
    m_voltageCorrectionFactorSpinBox->setSingleStep(0.0005);
    connect(m_voltageCorrectionFactorSpinBox,   static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,                               &AnalogInputWidget::voltageCorrectionFactorValueChanged);

    voltageLayout->addWidget(m_enable0_10VRadioButton);
    voltageLayout->addSpacing(5);
    voltageLayout->addWidget(voltageValueLabel);
    voltageLayout->addWidget(m_voltageLineEdit);
    voltageLayout->addWidget(voltageMeasureUnitLabel);
    voltageLayout->addSpacing(5);
    voltageLayout->addWidget(voltageCorrectionFactorLabel);
    voltageLayout->addWidget(m_voltageCorrectionFactorSpinBox);
    voltageLayout->addStretch(1);


    QHBoxLayout *amperageLayout = new QHBoxLayout;

    m_enable4_20mARadioButton = new QRadioButton("0..20 мА");
    m_enable4_20mARadioButton->setMinimumWidth(80);
    connect(m_enable4_20mARadioButton, &QRadioButton::toggled, this, &AnalogInputWidget::enable4_20mA);

    QLabel *amperageValueLabel = new QLabel("значение:");
    QLabel *amperageMeasureUnitLabel = new QLabel(QStringLiteral("мА"));
    amperageMeasureUnitLabel->setMinimumWidth(40);
    QLabel *amperageCorrectionFactorLabel = new QLabel("коэф. коррекции:");

    m_amperageLineEdit = new QLineEdit();
    m_amperageLineEdit->setEnabled(false);

    m_amperageCorrectionFactorSpinBox = new QDoubleSpinBox();
    m_amperageCorrectionFactorSpinBox->setMinimum(0.8);
    m_amperageCorrectionFactorSpinBox->setMaximum(1.2);
    m_amperageCorrectionFactorSpinBox->setDecimals(4);
    m_amperageCorrectionFactorSpinBox->setValue(amperageCorrectionFactor);
    m_amperageCorrectionFactorSpinBox->setSingleStep(0.0005);
    connect(m_amperageCorrectionFactorSpinBox,  static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,                               &AnalogInputWidget::amperageCorrectionFactorValueChanged);

    amperageLayout->addWidget(m_enable4_20mARadioButton);
    amperageLayout->addSpacing(5);
    amperageLayout->addWidget(amperageValueLabel);
    amperageLayout->addWidget(m_amperageLineEdit);
    amperageLayout->addWidget(amperageMeasureUnitLabel);
    amperageLayout->addSpacing(5);
    amperageLayout->addWidget(amperageCorrectionFactorLabel);
    amperageLayout->addWidget(m_amperageCorrectionFactorSpinBox);
    amperageLayout->addStretch(1);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addLayout(disableButtonLayot);
    groupBoxLayout->addLayout(voltageLayout);
    groupBoxLayout->addLayout(amperageLayout);

    QString inputName = QString("Аналоговый вход ") + QString::number(inputNumber);
    m_groupBox = new QGroupBox(inputName);
    m_groupBox->setLayout(groupBoxLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_groupBox);
    setLayout(mainLayout);

    IOServer *ioServer = IOServer::instance();
    if (ioServer) {
        m_disableInputVariable = ioServer->variableByAlias(disableInputAlias);
        m_recalculationVariable = ioServer->variableByAlias(recalculationAlias);
        m_inputValueVariable = ioServer->variableByAlias(inputValueAlias);
        if (m_inputValueVariable) {
            connect(m_inputValueVariable, &IVariable::valueChanged, this, &AnalogInputWidget::inputValueChanged);
        }
    }

    disableInput(true);
    init = false;
}

AnalogInputWidget::~AnalogInputWidget()
{
    auto settings = AppSettings::settings();
    settings->beginGroup(SettingsGroupKey);
    settings->setValue(QString(SettingsVoltageCorrectionFactorKey + QString::number(m_inputNumber)),
                       m_voltageCorrectionFactorSpinBox->value());
    settings->setValue(QString(SettingsAmperageCorrectionFactorKey + QString::number(m_inputNumber)),
                       m_amperageCorrectionFactorSpinBox->value());
    settings->endGroup();
}

void AnalogInputWidget::disableInput(bool checked)
{
    if (!checked)
        return;

    if (!m_disableInputVariable || !m_recalculationVariable || m_inputValueVariable)
        return;

    m_recalculationVariable->writeValue(false);
    m_disableInputVariable->writeValue(true);

    m_voltageLineEdit->clear();
    m_amperageLineEdit->clear();
    if (!init)
        OperatorClickReport::instance()->write(QString("Аттестация: отключить аналоговый вход \"%1\"").arg(m_inputValueVariable->alias()));
}

void AnalogInputWidget::enable0_10V(bool checked)
{
    m_amperageLineEdit->clear();

    if (!checked)
        return;

    if (!m_disableInputVariable || !m_recalculationVariable)
        return;

    m_disableInputVariable->writeValue(false);
    m_recalculationVariable->writeValue(false);


    if (!m_inputValueVariable)
        return;

    m_inputValueVariable->setValueStrategyType(IVariable::NumberStrategy);
    m_inputValueVariable->setMinValue(0);
    m_inputValueVariable->setMaxValue(10);
    m_inputValueVariable->setServerValueDataType(IVariable::Float32);
    m_inputValueVariable->setPrecision(4);

    if (VariableValueStrategy *valueStrategy = m_inputValueVariable->valueStrategy()) {
        const std::type_info &typeinfo = typeid(*valueStrategy);
        if (typeinfo == typeid(NumberVariableValueStrategy)) {
            NumberVariableValueStrategy *strategy = dynamic_cast<NumberVariableValueStrategy *>(valueStrategy);
            strategy->setCoefficientK(0.0024420);
            strategy->setCoefficientB(0.0);
        }
    }

    m_inputValueVariable->forceUpdateValue();
    m_inputValueVariable->readValue();
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: режим аналогового входа 0-10В"));
    inputValueChanged();
}

void AnalogInputWidget::enable4_20mA(bool checked)
{
    m_voltageLineEdit->clear();

    if (!checked)
        return;

    if (!m_disableInputVariable || !m_recalculationVariable)
        return;

    m_disableInputVariable->writeValue(false);
    m_recalculationVariable->writeValue(true);


    if (!m_inputValueVariable)
        return;


    m_inputValueVariable->setValueStrategyType(IVariable::NumberStrategy);
    m_inputValueVariable->setMinValue(0);
    m_inputValueVariable->setMaxValue(20);
    m_inputValueVariable->setServerValueDataType(IVariable::Float32);
    m_inputValueVariable->setPrecision(4);

    if (VariableValueStrategy *valueStrategy = m_inputValueVariable->valueStrategy()) {
        const std::type_info &typeinfo = typeid(*valueStrategy);
        if (typeinfo == typeid(NumberVariableValueStrategy)) {
            NumberVariableValueStrategy *strategy = dynamic_cast<NumberVariableValueStrategy *>(valueStrategy);
            strategy->setCoefficientK(0.0048840);
            strategy->setCoefficientB(0.0);
        }
    }

    m_inputValueVariable->forceUpdateValue();
    m_inputValueVariable->readValue();
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: режим аналогового входа 4-20мА"));
    inputValueChanged();
}

void AnalogInputWidget::inputValueChanged()
{
    if (!m_inputValueVariable)
        return;

    QVariant variableValue = m_inputValueVariable->value();
    bool ok;
    double doubleValue = variableValue.toDouble(&ok);


    if (disableOutputRadioButton->isChecked()) {
        return;
    } else if (m_enable0_10VRadioButton->isChecked()) {
        double correctedDoubleValue = doubleValue * m_voltageCorrectionFactorSpinBox->value();
        QString stringValue = SapKsu::outputToPrecision(correctedDoubleValue, 4);
        m_voltageLineEdit->setText(stringValue);
    } else if (m_enable4_20mARadioButton->isChecked()) {
        double correctedDoubleValue = doubleValue * m_amperageCorrectionFactorSpinBox->value();
        QString stringValue = SapKsu::outputToPrecision(correctedDoubleValue, 4);
        m_amperageLineEdit->setText(stringValue);
    }
}

void AnalogInputWidget::voltageCorrectionFactorValueChanged(double value)
{
    Q_UNUSED(value)
    inputValueChanged();
}

void AnalogInputWidget::amperageCorrectionFactorValueChanged(double value)
{
    Q_UNUSED(value)
    inputValueChanged();
}
