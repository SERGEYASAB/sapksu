#include "AttestationWindow.h"

#include "AnalogOutputsContainerWidget.h"
#include "AnalogInputsContainerWidget.h"
#include "DigitalOutputsContainerWidget.h"
#include "DigitalInputsContainerWidget.h"
#include "PowerSupplyWidget.h"
#include "AttestationEnergoform.h"

#include "IOServer/IOServer.h"
#include "StartWindow/StartWindow.h"
#include "IOServer/nsVariable/IVariable.h"
#include "ReportManager/OperatorClickReport.h"

AttestationWindow::AttestationWindow(QWidget *parent) : QTabWidget(parent)
{
    addTab(new AnalogOutputsContainerWidget(), QStringLiteral("Аналоговые выходы"));
    addTab(new AnalogInputsContainerWidget(), QStringLiteral("Аналоговые входы"));
    addTab(new DigitalOutputsContainerWidget(), QStringLiteral("Цифровые выходы"));
    addTab(new DigitalInputsContainerWidget(), QStringLiteral("Цифровые входы"));
    addTab(new PowerSupplyWidget(), QStringLiteral("Выходы питания"));
    addTab(new AttestationEnergoform(), QStringLiteral("Энергоформа/энергомонитор"));

    connect(this, &QTabWidget::currentChanged, this, &AttestationWindow::currentIndexChanged);


    IOServer *ioServer = IOServer::instance();
    if (ioServer) {
        m_enableDigitalOutputs1Variable = ioServer->variableByAlias("$A15_B6");
        m_enableDigitalOutputs2Variable = ioServer->variableByAlias("$A15_B7");
        m_enableDigitalInputsVariable = ioServer->variableByAlias("$A16_A0");

        IVariable *enablePhaseAVariable = ioServer->variableByAlias("$SAP_PHASE_A_ENABLE");
        if (enablePhaseAVariable) {
            enablePhaseAVariable->writeValue(true);
        }
        IVariable *enablePhaseBVariable = ioServer->variableByAlias("$SAP_PHASE_B_ENABLE");
        if (enablePhaseBVariable) {
            enablePhaseBVariable->writeValue(true);
        }
        IVariable *enablePhaseCVariable = ioServer->variableByAlias("$SAP_PHASE_C_ENABLE");
        if (enablePhaseCVariable) {
            enablePhaseCVariable->writeValue(true);
        }
    }
}

void AttestationWindow::closeEvent(QCloseEvent *event)
{
    IOServer::deleteIOServer();

    StartWindow *startWindow = new StartWindow;
    startWindow->setAttribute(Qt::WA_DeleteOnClose);
    startWindow->setWindowModality(Qt::NonModal);
    startWindow->setWindowTitle("САП КСУ - Стенд автоматизированной проверки контроллеров станции управления");
    startWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
    startWindow->resize(QSize(1024, 768));
    startWindow->move(QApplication::desktop()->screenGeometry().center() - startWindow->rect().center());
    startWindow->show();
    startWindow->activateWindow();
    startWindow->showNormal();    
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: закрытие окна"));
    event->accept();
}

void AttestationWindow::currentIndexChanged(int index)
{
    if (index == 2) { /// Цифровые выходы
        if (m_enableDigitalOutputs1Variable)
            m_enableDigitalOutputs1Variable->writeValue(true);
        if (m_enableDigitalOutputs2Variable)
            m_enableDigitalOutputs2Variable->writeValue(true);
    } else {
        if (m_enableDigitalOutputs1Variable)
            m_enableDigitalOutputs1Variable->writeValue(false);
        if (m_enableDigitalOutputs2Variable)
            m_enableDigitalOutputs2Variable->writeValue(false);
    }

    if (index == 3) { /// Цифровые входы
        if (m_enableDigitalInputsVariable)
            m_enableDigitalInputsVariable->writeValue(true);
    } else {
        if (m_enableDigitalInputsVariable)
            m_enableDigitalInputsVariable->writeValue(false);
    }
}
