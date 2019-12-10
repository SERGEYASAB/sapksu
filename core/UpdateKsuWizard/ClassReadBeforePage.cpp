#include "ClassReadBeforePage.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsNode/ModbusKsuNode.h"
#include "ManualTesting/ModbusKsu/ModbusKsuManualFullTesting/ModbusKsuManualFullTestingWidget.h"
#include "ManualTesting/Ksu735/ManualFullTesting/Ksu735ManualFullTestingWidget.h"
#include "Common/QMessageBox/ScriptDelayMessageBox/ProgressBarMessageBox.h"
#include "Logger/KsuLogWidget/KsuLogWidget.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsVariable/ModbusVariable.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

ClassReadBeforePage::ClassReadBeforePage(QWidget *parent) : QWizardPage(parent)
{
    init();
}

void ClassReadBeforePage::initializePage()
{
    IOServer *ioServer = IOServer::instance();
    if (!ioServer)
        return;

    IVariable *variable;
    int workState = 0;

    if (Ksu735Node *ksu735Node = qobject_cast<Ksu735Node *>(ioServer->ksuNode())) {

        variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU735x10_status_word"));
        workState = variable->readValue().toInt() & 0x8000;

    } else if (ModbusKsuNode *modbusKsuNode = qobject_cast<ModbusKsuNode *>(ioServer->ksuNode())) {

        variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_STATUS_WORD"));
        workState = variable->readValue().toInt() & 0x0001;
    }

    if (workState) {
        pedState = "работает";
    } else {
        pedState = "стоит";
    }

    if (m_tempMbTestingWidget) {
        QTimer::singleShot(100, m_tempMbTestingWidget, SLOT(readAll()));
        emit started();
    } else if (m_temp735TestingWidget) {
        QTimer::singleShot(100, m_temp735TestingWidget, SLOT(readAll()));
        emit started();
    }
}

QString ClassReadBeforePage::getPedState() const
{
    return pedState;
}

void ClassReadBeforePage::setCompleted(bool state)
{
    m_completed = state;
    emit completeChanged();
}

void ClassReadBeforePage::init()
{
    INode *node = IOServer::instance()->ksuNode();

    ModbusKsuNode *mbNode = qobject_cast<ModbusKsuNode *>(node);

    if (mbNode) {
        m_tempMbTestingWidget = new ModbusKsuManualFullTestingWidget(mbNode);
        m_tempMbTestingWidget->setAutotestingmodeFlag(true);
    }

    Ksu735Node *n735Node = qobject_cast<Ksu735Node *>(node);

    if (n735Node) {
        m_temp735TestingWidget = new Ksu735ManualFullTestingWidget(n735Node);
        m_temp735TestingWidget->setAutotestingmodeFlag(true);
    }

    setSubTitle("Чтение параметров, подождите...");
    setButtonText(QWizard::NextButton, "Далее");
    setButtonText(QWizard::CancelButton, "Отмена");

    KsuLogWidget *logger = new KsuLogWidget(10);

    QGridLayout *layout = new QGridLayout;

    if (m_tempMbTestingWidget) {

        layout->addWidget(m_tempMbTestingWidget->getProgressBarMessageBox(), 0, 0);

        connect(m_tempMbTestingWidget, &ModbusKsuManualFullTestingWidget::progressBarChanged,
                [this](int value)
        {
            if (value >= 100) {
                QList<IVariable *> vars;
                for (auto i : m_tempMbTestingWidget->enabledParametrs())
                    vars.append(static_cast<IVariable *>(i->variable()));
                emit finished(vars);
                setCompleted(true);
            }
            else
                setCompleted(false);
        });
    } else if (m_temp735TestingWidget) {

        layout->addWidget(m_temp735TestingWidget->getProgressBarMessageBox(), 0, 0);

        connect(m_temp735TestingWidget, &Ksu735ManualFullTestingWidget::progressBarChanged,
                [this](int value)
        {
            if (value >= 100) {
                QList<IVariable *> vars;
                for (auto i : m_temp735TestingWidget->enabledParametrs())
                    vars.append(static_cast<IVariable *>(i->variable()));
                emit finished(vars);
                setCompleted(true);
            }
            else
                setCompleted(false);
        });
    }

    connect(this, &ClassReadBeforePage::completeChanged, [this]()
    {
        if (m_completed == true) {
            setSubTitle("Чтение параметров завершено.");
        }
    });

    layout->addWidget(logger, 1, 0);
    setLayout(layout);
}

bool ClassReadBeforePage::isComplete() const
{
    return m_completed;
}

void ClassReadBeforePage::cancelTesting()
{
    if (m_tempMbTestingWidget) {
        m_tempMbTestingWidget->stopTesting();
    } else if (m_temp735TestingWidget) {
        m_temp735TestingWidget->stopTesting();
    }
}
