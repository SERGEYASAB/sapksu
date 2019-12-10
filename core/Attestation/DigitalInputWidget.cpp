#include "DigitalInputWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "ReportManager/OperatorClickReport.h"

DigitalInputWidget::DigitalInputWidget(const QString &inputName, const QString &inputAlias, QWidget *parent) : QWidget(parent)
{
    m_inputStateIconLabel = new QLabel();
    m_inputStateTextLabel = new QLabel();

    QHBoxLayout *groupBoxLayout = new QHBoxLayout();
    groupBoxLayout->addWidget(m_inputStateIconLabel);
    groupBoxLayout->addWidget(m_inputStateTextLabel);
    groupBoxLayout->addStretch(1);

    m_groupBox = new QGroupBox(inputName);
    m_groupBox->setLayout(groupBoxLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_groupBox);
    setLayout(mainLayout);

    QIcon disabledIcon(":/Images/bulbDigitalInputDisabled.png");
    m_disabledPixmap = disabledIcon.pixmap(24);

    QIcon enabledIcon(":/Images/bulbDigitalInputEnabled.png");
    m_enabledPixmap = enabledIcon.pixmap(24);

    IOServer *ioServer = IOServer::instance();
    if (ioServer) {
        m_inputVariable = ioServer->variableByAlias(inputAlias);
        if (m_inputVariable) {
            connect(m_inputVariable, &IVariable::valueChanged, this, &DigitalInputWidget::testInputValue);
        }
    }

    testInputValue();
}

void DigitalInputWidget::testInputValue()
{
    if (m_inputVariable && m_inputVariable->value().toBool()) {
        m_inputStateIconLabel->setPixmap(m_enabledPixmap);
        m_inputStateTextLabel->setText(QStringLiteral("включен"));

    } else {
        m_inputStateIconLabel->setPixmap(m_disabledPixmap);
        m_inputStateTextLabel->setText(QStringLiteral("отключен"));
    }
}
