#include "ExecutorSimpleWindow.h"
#include "ITest.h"

ExecutorSimpleWindow::ExecutorSimpleWindow(const QList<ITest *> &tests, QWidget *parent)
    : QMainWindow(parent),
      m_tests(tests)
{
        setupToolBar();

}

ExecutorSimpleWindow::~ExecutorSimpleWindow()
{

}

void ExecutorSimpleWindow::setupToolBar()
{
    m_toolBar = new QToolBar(tr("Управление тестами"));
    m_toolBar->setObjectName("m_toolBar");

    const QIcon runTestIcon = QIcon(":/Images/media-seek-forward.png");
    QAction *runTestAction = new QAction(runTestIcon, tr("&Выполнить тест до конца"), this);
    runTestAction->setStatusTip(tr("Выполнить тест до конца"));
    connect(runTestAction, &QAction::triggered, this, &ExecutorSimpleWindow::runCurrentTest);

    const QIcon runStepAndGoNextIcon = QIcon(":/Images/media-skip-forward.png");
    QAction *runStepAndGoNextAction = new QAction(runStepAndGoNextIcon,
                                                  tr("&Выполнить текущий шаг и перейти к следующему"), this);
    runStepAndGoNextAction->setStatusTip(tr("Выполнить текущий шаг и перейти к следующему"));
    connect(runStepAndGoNextAction, &QAction::triggered, this, &ExecutorSimpleWindow::runCurrentStepAndGoNext);

    const QIcon runStepRetryIcon = QIcon(":/Images/media-playback-start.png");
    QAction *runStepRetryAction = new QAction(runStepRetryIcon,
                                              tr("&Выполнить текущий шаг повторно"), this);
    runStepRetryAction->setStatusTip(tr("Выполнить текущий шаг повторно"));
    connect(runStepRetryAction, &QAction::triggered, this, &ExecutorSimpleWindow::runCurrentStep);

    m_toolBar->addAction(runTestAction);
    m_toolBar->addAction(runStepAndGoNextAction);
    m_toolBar->addAction(runStepRetryAction);

    addToolBar(Qt::TopToolBarArea, m_toolBar);
}

ITest *ExecutorSimpleWindow::currentTest() const
{
    return m_currentTest;
}

void ExecutorSimpleWindow::setCurrentTest(ITest *currentTest)
{
    if (m_currentTest == currentTest)
        return;

//    if (m_currentTest != nullptr) {

//        disconnect(m_currentTest, &ITest::messageChanged, this, &ExecutorWindow::logMessage);
//        disconnect(m_currentTest, &ITest::currentStepChanged, this, &ExecutorWindow::updateStepListWidget);
//        disconnect(currentTest, &ITest::currentTestDeinitAndGoNext, this, &ExecutorWindow::goNextTest);
//    }

    m_currentTest = currentTest;

//    connect(currentTest, &ITest::messageChanged, this, &ExecutorWindow::logMessage);
//    connect(currentTest, &ITest::currentStepChanged, this, &ExecutorWindow::updateStepListWidget);
//    connect(currentTest, &ITest::currentTestDeinitAndGoNext, this, &ExecutorWindow::goNextTest);

//    updateTestListWidget(currentTest);

//    fillVarTableWidgets();
//    fillStepListWidget();

}

bool ExecutorSimpleWindow::runStep(int stepNumber)
{
    currentTest()->runStep(stepNumber);
}

void ExecutorSimpleWindow::runStepAndGoNext(int stepNumber)
{
    currentTest()->runStepAndGoNext(stepNumber);
}

void ExecutorSimpleWindow::goNextTest()
{
    if (m_tests.last() != currentTest()) {
        int indexCurrentTest = m_tests.indexOf(currentTest());
        setCurrentTest(m_tests.at(indexCurrentTest + 1));
    }
}

void ExecutorSimpleWindow::runCurrentStep()
{
    runStep(currentStepNumber);
}

void ExecutorSimpleWindow::runCurrentStepAndGoNext()
{
    runStepAndGoNext(currentStepNumber);
}

void ExecutorSimpleWindow::runCurrentTest()
{
    currentTest()->runTest();
}

void ExecutorSimpleWindow::logMessage(const QString &message)
{
//    m_logTextEdit->append(message);
}

