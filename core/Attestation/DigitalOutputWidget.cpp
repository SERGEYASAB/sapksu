#include "DigitalOutputWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "ReportManager/OperatorClickReport.h"

DigitalOutputWidget::DigitalOutputWidget(const QString &outputName, const QString &outputAlias, QWidget *parent) : QWidget(parent)
{
    m_disableRadioButton = new QRadioButton("отключить");
    m_disableRadioButton->setChecked(true);
    connect(m_disableRadioButton, &QRadioButton::toggled, this, &DigitalOutputWidget::disableOutput);

    m_enableRadioButton = new QRadioButton("включить");
    connect(m_enableRadioButton, &QRadioButton::toggled, this, &DigitalOutputWidget::enableOutput);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addWidget(m_disableRadioButton);
    groupBoxLayout->addWidget(m_enableRadioButton);

    m_groupBox = new QGroupBox(outputName);
    m_groupBox->setLayout(groupBoxLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_groupBox);
    setLayout(mainLayout);

    IOServer *ioServer = IOServer::instance();
    if (ioServer) {
        m_outputVariable = ioServer->variableByAlias(outputAlias);
        //        connect(m_inputVariable, &IVariable::valueChanged, this, &DigitalInputWidget::testInputValue);
    }

    disableOutput(true);
    init = false;
}

void DigitalOutputWidget::disableOutput(bool checked)
{
    if (!checked)
        return;

    if (!m_outputVariable)
        return;

    m_outputVariable->writeValue(false);
    if (!init)
        OperatorClickReport::instance()->write(QString("Аттестация: отключить выход \"%1\"").arg(m_outputVariable->alias()));
}

void DigitalOutputWidget::enableOutput(bool checked)
{
    if (!checked)
        return;

    if (!m_outputVariable)
        return;

    m_outputVariable->writeValue(true);
    OperatorClickReport::instance()->write(QString("Аттестация: включить выход \"%1\"").arg(m_outputVariable->alias()));
}
