#include "ExecutorWindow.h"
#include "ITest.h"
#include "ITestStep.h"
#include "Common/QPlainTextEdit/CodeEditorPlainTextEdit/CodeEditorPlainTextEdit.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/IOServer.h"
#include "Devices/DeviceModel.h"
#include "Devices/DeviceDelegate.h"
#include "Common/QSyntaxHighlighter/JSSyntaxHighlighter/JSSyntaxHighlighter.h"
#include "TestExternalVariableWidget.h"
#include "TestExternalVariableModel.h"
#include "TestInternalVariableWidget.h"
#include "TestInternalVariableModel.h"
#include "AppSettings/AppSettings.h"
#include "ReportManager/ReportConfigure.h"
#include "StartWindow/ActionControlWidgetsShow.h"
#include "Logger/KsuLogWidget/KsuLogWidget.h"
#include "ReportManager/OperatorClickReport.h"
#include "TestManager/JSTest.h"
#include "TestManager/JSTestScriptEngine.h"

ExecutorWindow::ExecutorWindow(const QList<ITest *> &tests, QWidget *parent)
    : QMainWindow(parent),
      m_tests(tests)
{
    setupToolBar();
    setupLogDockWidget();
    setupTestListDockWidget();
    setupStepListDockWidget();
    setupLoggerOutputWidget();
    setupTextProgramWidget();
    setupExtVarDockWidget();
    setupKsuVarDockWidget();
    setupIntVarDockWidget();
    fillTestListWidget();
    tabifyDockWidget(m_logDock, m_logDockWidget);
    setupMainWindowSettings();

    m_reportConfig = new ReportConfigure();
    m_reportConfig->setWindowModality(Qt::ApplicationModal);
    m_reportConfig->setWindowTitle("Отчеты");
    m_reportConfig->setWindowIcon(QIcon(":/Images/report.png"));
}

ExecutorWindow::~ExecutorWindow()
{
    if (m_reportConfig->isVisible())
        m_reportConfig->close();
    delete m_reportConfig;
}

ITest *ExecutorWindow::currentTest() const
{
    return m_currentTest;
}

void ExecutorWindow::setCurrentTest(ITest *currentTest)
{
    if (m_currentTest == currentTest)
        return;

    if (m_currentTest != nullptr) {

        disconnect(m_currentTest, &ITest::messageChanged, this, &ExecutorWindow::logMessage);
        disconnect(m_currentTest, &ITest::currentStepChanged, this, &ExecutorWindow::updateStepListWidget);
        disconnect(m_currentTest, &ITest::currentTestDeinitAndGoNext, this, &ExecutorWindow::goNextTest);
    }

    m_currentTest = currentTest;

    connect(currentTest, &ITest::messageChanged, this, &ExecutorWindow::logMessage);
    connect(currentTest, &ITest::currentStepChanged, this, &ExecutorWindow::updateStepListWidget);
    connect(currentTest, &ITest::currentTestDeinitAndGoNext, this, &ExecutorWindow::goNextTest);

    updateTestListWidget(currentTest);

    fillVarTableWidgets();
    fillStepListWidget();

}

void ExecutorWindow::setupTestListDockWidget()
{
    m_testListWidget = new QListWidget;
#ifdef QT_NO_DEBUG
    m_testListWidget->setDisabled(true);
#endif

    QDockWidget *testListDockWidget = new QDockWidget(tr("Список тестов"));
    testListDockWidget->setWidget(m_testListWidget);
    testListDockWidget->setObjectName("testListDockWidget");
    testListDockWidget->setDisabled(true);

    addDockWidget(Qt::LeftDockWidgetArea, testListDockWidget);
}

void ExecutorWindow::setupStepListDockWidget()
{
    m_stepListWidget = new QListWidget;
#ifdef QT_NO_DEBUG
    m_stepListWidget->setDisabled(true);
#endif

    QDockWidget *stepListDockWidget = new QDockWidget(tr("Список шагов"));
    stepListDockWidget->setWidget(m_stepListWidget);
    stepListDockWidget->setObjectName("stepListDockWidget");
    stepListDockWidget->setDisabled(true);

    addDockWidget(Qt::LeftDockWidgetArea, stepListDockWidget);
}

void ExecutorWindow::setupTextProgramWidget()
{
    m_textProgramWidget = new CodeEditorPlainTextEdit(this);
#ifdef QT_NO_DEBUG
    m_textProgramWidget->setReadOnly(true);
#endif

    JSSyntaxHighlighter *painter = new JSSyntaxHighlighter(m_textProgramWidget->document());

    setCentralWidget(m_textProgramWidget);
}

void ExecutorWindow::setupLoggerOutputWidget()
{
    //    m_logTextEdit = new QScriptEngineDebugger;
    //    m_logTextEdit->setAutoShowStandardWindow(false);

    //    QDockWidget *debugOutputWidget = new QDockWidget(tr("История..."));

    //    debugOutputWidget->setWidget(m_logTextEdit->widget(QScriptEngineDebugger::DebugOutputWidget));

    m_logTextEdit = new QTextEdit;
    m_logTextEdit->setReadOnly(true);
    m_logDockWidget = new QDockWidget(tr("История..."));
    m_logDockWidget->setObjectName("logDockWidget");
    m_logDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    //#ifdef QT_NO_DEBUG
    m_logDockWidget->setWidget(m_logTextEdit);
    //#endif
    addDockWidget(Qt::BottomDockWidgetArea, m_logDockWidget);
}

void ExecutorWindow::setupExtVarDockWidget()
{
    extVarWidget = new TestExternalVariableWidget(new DeviceDelegate, this);
    QDockWidget *extVarDockWidget = new QDockWidget(tr("Глобальные переменные"));
    extVarDockWidget->setWidget(extVarWidget);
    extVarDockWidget->setObjectName("extVarDockWidget");

    addDockWidget(Qt::RightDockWidgetArea, extVarDockWidget);
}

void ExecutorWindow::setupKsuVarDockWidget()
{
    ksuVarWidget = new TestExternalVariableWidget(new DeviceDelegate, this);
    QDockWidget *ksuVarDockWidget = new QDockWidget(tr("Переменные КСУ"));
    ksuVarDockWidget->setWidget(ksuVarWidget);
    ksuVarDockWidget->setObjectName("ksuVarDockWidget");

    addDockWidget(Qt::RightDockWidgetArea, ksuVarDockWidget);
}

void ExecutorWindow::setupIntVarDockWidget()
{
    intVarWidget = new TestInternalVariableWidget(nullptr, this);
    QDockWidget *intVarDockWidget = new QDockWidget(tr("Внутренние переменные теста"));
    intVarDockWidget->setWidget(intVarWidget);
    intVarDockWidget->setObjectName("intVarDockWidget");

    addDockWidget(Qt::RightDockWidgetArea, intVarDockWidget);
}

void ExecutorWindow::setupToolBar()
{
    m_toolBar = new QToolBar(tr("Управление тестами"));
    m_toolBar->setObjectName("m_toolBar");

    const QIcon runAllTestIcon = QIcon(":/Images/automaticTesting.png");
    QAction *runAllTestAction = new QAction(runAllTestIcon, tr("&Выполнить все тесты"), this);
    runAllTestAction->setStatusTip(tr("Выполнить все тесты до конца"));
    connect(runAllTestAction, &QAction::triggered, this, &ExecutorWindow::runAllTests);

    const QIcon runTestIcon = QIcon(":/Images/media-seek-forward.png");
    QAction *runTestAction = new QAction(runTestIcon, tr("&Выполнить тест до конца"), this);
    runTestAction->setStatusTip(tr("Выполнить тест до конца"));
    connect(runTestAction, &QAction::triggered, this, &ExecutorWindow::runCurrentTest);

    const QIcon runStepAndGoNextIcon = QIcon(":/Images/media-skip-forward.png");
    QAction *runStepAndGoNextAction = new QAction(runStepAndGoNextIcon,
                                                  tr("&Выполнить текущий шаг и перейти к следующему"), this);
    runStepAndGoNextAction->setStatusTip(tr("Выполнить текущий шаг и перейти к следующему"));
    connect(runStepAndGoNextAction, &QAction::triggered, this, &ExecutorWindow::runCurrentStepAndGoNext);

    const QIcon runStepRetryIcon = QIcon(":/Images/media-playback-start.png");
    QAction *runStepRetryAction = new QAction(runStepRetryIcon,
                                              tr("&Выполнить текущий шаг повторно"), this);
    runStepRetryAction->setStatusTip(tr("Выполнить текущий шаг повторно"));
    connect(runStepRetryAction, &QAction::triggered, this, &ExecutorWindow::runCurrentStep);

    m_stepNumberSpinBox = new QSpinBox;

    const QIcon runStepBeforeStepNumberIcon = QIcon(":/Images/media-playback-start.png");
    QAction *runStepBeforeStepNumberAction = new QAction(runStepBeforeStepNumberIcon,
                                                         tr("Выполнить тест до указанного шага"), this);
    runStepBeforeStepNumberAction->setStatusTip(tr("Выполнить тест до указанного шага"));
    connect(runStepBeforeStepNumberAction, &QAction::triggered, [this]()
    {
        runCurrenTestBeforeStepNumber(m_stepNumberSpinBox->value());
        OperatorClickReport::instance()->write(QStringLiteral("Автоматическое тестирование: нажата кнопка \"Выполнить тест до указанного шага\""));
    });

    const QIcon restartTestingIcon = QIcon(":/Images/text-x-bak.png");
    QAction *restartTestingAction = new QAction(restartTestingIcon,
                                                tr("&Начать тестирование заново"), this);
    restartTestingAction->setStatusTip(tr("Переход к первому шагу первого теста"));
    connect(restartTestingAction, &QAction::triggered, this, &ExecutorWindow::restartTestingPrepare);

    const QIcon showReportIcon = QIcon(":/Images/report.png");
    QAction *showReportAction = new QAction(showReportIcon, tr("Отчеты по тестированиям"), this);
    showReportAction->setStatusTip(tr("Запустить окно для построения отчета"));
    connect(showReportAction, &QAction::triggered, this, &ExecutorWindow::showReportWindow);

    const QIcon previewReportIcon = QIcon(":/Images/print.png");
    QAction *previewReportAction = new QAction(previewReportIcon, tr("Распечатать отчет"), this);
    previewReportAction->setStatusTip(tr("Распечатать отчет по текущему тестированию"));
    connect(previewReportAction, &QAction::triggered, this, &ExecutorWindow::previewReportWindow);

    const QIcon pdfIcon = QIcon(":/Images/pdf.png");
    QAction *pdfAction = new QAction(pdfIcon, tr("Распечатать отчет"), this);
    pdfAction->setStatusTip(tr("Распечатать отчет по текущему тестированию"));
    connect(pdfAction, &QAction::triggered, this, &ExecutorWindow::printToPdf);

    m_toolBar->addAction(runAllTestAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(runTestAction);
    m_toolBar->addAction(runStepAndGoNextAction);
    m_toolBar->addAction(runStepRetryAction);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(new QLabel("Выполнить тест до указанного шага: "));
    m_toolBar->addWidget(m_stepNumberSpinBox);
    m_toolBar->addAction(runStepBeforeStepNumberAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(restartTestingAction);

    // Временно скрываем программные кнопки пауза и продолжить (реализация не доделана)
    /*
#ifdef QT_DEBUG

    const QIcon sendPauseIcon = QIcon(":/Images/pause.png");
    QAction *sendPauseAction = new QAction(sendPauseIcon, tr("Пауза"), this);
    sendPauseAction->setStatusTip(tr("Пауза - имитация нажатия кнопки"));
    connect(sendPauseAction, &QAction::triggered, this, &ExecutorWindow::sendPause);

    const QIcon sendContinueIcon = QIcon(":/Images/updateksu.png");
    QAction *sendContinueAction = new QAction(sendContinueIcon, tr("Продолжить"), this);
    sendContinueAction->setStatusTip(tr("Продолжить - имитация нажатия кнопки"));
    connect(sendContinueAction, &QAction::triggered, this, &ExecutorWindow::sendContinue);

    m_toolBar->addSeparator();
    m_toolBar->addAction(sendPauseAction);
    m_toolBar->addAction(sendContinueAction);
#endif
*/

    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    m_pauseStateLabel = new QLabel("Нажата пауза! - Для продолжение нажимите кнопку \"продолжить\"");
    m_pauseStateLabel-> setStyleSheet("background-color: red");
    m_pauseStateLabel->setVisible(false);

    m_toolBar->addWidget(empty);

    // Note: You should use QAction::setVisible() to change the visibility of the widget.
    // Using QWidget::setVisible(), QWidget::show() and QWidget::hide() does not work.
    m_pauseStateLabelAction = m_toolBar->addWidget(m_pauseStateLabel);
    connect(IOServer::instance(), &IOServer::pauseStateChanged, m_pauseStateLabelAction, &QAction::setVisible);

    m_toolBar->addSeparator();
    m_toolBar->addAction(showReportAction);
    m_toolBar->addAction(previewReportAction);
    m_toolBar->addAction(pdfAction);

    addToolBar(Qt::TopToolBarArea, m_toolBar);
}

void ExecutorWindow::setupLogDockWidget()
{
    m_ksuLogWidget = new KsuLogWidget();
    m_logDock = new QDockWidget(tr("Лог обмена данными с КСУ"));
    m_logDock->setObjectName(QStringLiteral("ksuAutoLogDockWidget"));
    m_logDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    m_logDock->setWidget(m_ksuLogWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_logDock);
}

void ExecutorWindow::updateTestListWidget(ITest *currentTest)
{
    m_testListWidget->setCurrentRow(m_tests.indexOf(currentTest));
}

void ExecutorWindow::updateStepListWidget(ITestStep *currentStep)
{
    m_stepListWidget->setCurrentRow(currentTest()->steps().indexOf(currentStep));
    updateProgramWidget(currentStep);
}

void ExecutorWindow::updateProgramWidget(ITestStep *currentStep)
{
    m_textProgramWidget->setPlainText(currentStep->textProgram());
}

void ExecutorWindow::readSettings()
{
    auto settings = AppSettings::settings();
    restoreGeometry(settings->value(QString(metaObject()->className()) + "/geometry").toByteArray());
    restoreState(settings->value(QString(metaObject()->className()) + "/windowState").toByteArray());
}

void ExecutorWindow::setupMainWindowSettings()
{
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);

    const QIcon iconWindow = QIcon(":/Images/batma-19.png");
    setWindowIcon(iconWindow);
    setWindowTitle(tr("\"Запускатор тестов\""));
}

bool ExecutorWindow::checkPressContinue()
{
    if (IOServer::instance()->getPauseTestStopButton()) {

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Блокировка управления");
        msgBox.setText(tr("Операция заблокирована т.к. была нажата кнопка \"Пауза\""));
        msgBox.setInformativeText("Для продолжения работы нажмите кнопку \"Продолжить\" и выполните операцию повторно");
        msgBox.setMinimumWidth(400);

        QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
        if (layout)
            layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();

        m_pauseStateLabel->setVisible(true);

        return true;
    }

    return false;
}

void ExecutorWindow::sendPause()
{
    IVariable *variable = IOServer::instance()->variableByAlias("$SAP_PAUSE");

    if (!variable)
        return;

    variable->setValue(true);

    emit variable->valueChanged(variable);

    qDebug() << variable->value();
}

void ExecutorWindow::sendContinue()
{
    IVariable *variable = IOServer::instance()->variableByAlias("$SAP_CONTINUE");

    if (!variable)
        return;

    variable->setValue(true);

    emit variable->valueChanged(variable);

    qDebug() << variable->value();

}

void ExecutorWindow::fillTestListWidget()
{
    if (m_tests.isEmpty())
        return;

    for (auto test : m_tests) {
        m_testListWidget->addItem(test->name());
    }

    setCurrentTest(m_tests.first());
}

void ExecutorWindow::fillStepListWidget()
{
    m_stepListWidget->clear();

    if (!currentTest() || currentTest()->steps().isEmpty())
        return;

    int stepNumber = 0;

    for (auto step : currentTest()->steps()) {

        m_stepListWidget->addItem(QString::number(stepNumber++) + "_" + step->name());
    }

    updateStepListWidget(currentTest()->steps().first());
}

void ExecutorWindow::fillVarTableWidgets()
{
    if (currentTest()) {
        QList<IVariable*> extVars, ksuVars;
        QList<EngineLocalVariable*> intVars;

        for (auto i : currentTest()->externalVariables()){
            if (i->alias().startsWith('$'))
                extVars.append(i);
            else
                ksuVars.append(i);
        }
        extVarWidget->setData(extVars);
        ksuVarWidget->setData(ksuVars);

        for (auto i : currentTest()->internalVariables())
            intVars.append(i);
        intVarWidget->setData(intVars);
    }
}

bool ExecutorWindow::runStep(int stepNumber)
{
    if (currentTest()) {
        currentTest()->runStep(stepNumber);
        return true;
    }
    return false;
}

void ExecutorWindow::runStepAndGoNext(int stepNumber)
{
    if (currentTest())
        currentTest()->runStepAndGoNext(stepNumber);
}

void ExecutorWindow::goNextTest()
{
    if (m_tests.last() == currentTest() && currentTest()->testStatus() == ITest::TestStatus::Deinit) {
        QMessageBox::information(this, "Информация", "Тестирование завершено. \nПодробные сведения о проведенном тестировании смотрите в разделе \"Отчеты\".");
        m_testFinished = true;
    }

    if (m_tests.last() != currentTest() && currentTest()->testStatus() == ITest::TestStatus::Deinit) {
        int indexCurrentTest = m_tests.indexOf(currentTest());
        setCurrentTest(m_tests.at(indexCurrentTest + 1));
    }
    OperatorClickReport::instance()->write(QStringLiteral("Автоматическое тестирование: нажата кнопка \"Перейти к следующему тесту\""));
}

void ExecutorWindow::runCurrentStep()
{
    if (checkPressContinue())
        return;

    runStep(m_stepListWidget->currentRow());
    OperatorClickReport::instance()->write(QStringLiteral("Автоматическое тестирование: нажата кнопка \"Выполнить текущий шаг\""));
}

void ExecutorWindow::runCurrentStepAndGoNext()
{
    if (checkPressContinue())
        return;

    runStepAndGoNext(m_stepListWidget->currentRow());
    OperatorClickReport::instance()->write(QStringLiteral("Автоматическое тестирование: нажата кнопка \"Выполнить текущий шаг и перейти к следующему\""));
}

void ExecutorWindow::runCurrentTest()
{
    if (checkPressContinue() || !currentTest())
        return;

    m_testFinished = false;
    currentTest()->runTest();
    OperatorClickReport::instance()->write(QStringLiteral("Автоматическое тестирование: нажата кнопка \"Запустить текущий тест\""));
}

void ExecutorWindow::runAllTests()
{
    if (currentTest() == m_tests.last() && currentTest()->testStatus() == ITest::Deinit) {
        QMessageBox::warning(this, "Предупреждение", "Текущее тестирование завершено. \nЧтобы начать тестирование заново, нажмите кнопку \"Начать тестирование заново\" для сброса текущего состояния, после чего выберите дальнейшее действие.", QMessageBox::Ok);
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Выбор состояния");
#ifdef QT_DEBUG
    msgBox.setWindowModality(Qt::NonModal);
#endif
    msgBox.setText("Вы действительно хотите выполнить тесты целиком?");
    msgBox.setWindowIcon(QIcon(":/Images/irz_logo.png"));
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *yesButton = msgBox.addButton("Да", QMessageBox::AcceptRole);
    QPushButton *noButton = msgBox.addButton("Нет", QMessageBox::RejectRole);
    msgBox.setDefaultButton(noButton);

    int ret = msgBox.exec();

    if (ret == QMessageBox::AcceptRole) {

        for (auto test : m_tests) {
            if (checkPressContinue())
                break;

            Q_UNUSED(test);
            runCurrentTest();
        }
    }
}

void ExecutorWindow::runCurrenTestBeforeStepNumber(int stepNumber)
{
    if (checkPressContinue() || !currentTest())
        return;

    currentTest()->runTestBeforeStepNumber(stepNumber);
}

void ExecutorWindow::executeTest(ITest *test)
{

}

void ExecutorWindow::logMessage(const QString &message)
{
    m_logTextEdit->append(message);
}

void ExecutorWindow::showReportWindow()
{
    QDateTime to = QDateTime::currentDateTime();
    m_reportConfig->showReport(ReportConfigure::TestHistoryReport, m_beginDate, to);
}

void ExecutorWindow::previewReportWindow()
{
    QDateTime to = QDateTime::currentDateTime();
    m_reportConfig->previewReport(ReportConfigure::TestHistoryReport, m_beginDate, to);
}

void ExecutorWindow::printToPdf()
{
    QDateTime to = QDateTime::currentDateTime();
    m_reportConfig->printToPdfReport(ReportConfigure::TestHistoryReport, m_beginDate, to);
}

void ExecutorWindow::closeEvent(QCloseEvent *event)
{
    auto settings = AppSettings::settings();
    settings->beginGroup(metaObject()->className());
    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());
    settings->endGroup();
    settings->sync();

    if (JSTest *test = qobject_cast<JSTest *>(currentTest())) {
        test->engine()->setAbortFlag(true);
    }

    QMainWindow::closeEvent(event);
}

void ExecutorWindow::restartTestingPrepare()
{
    if (QMessageBox::question(this, "Начать тестирование заново", "Текущее состояние тестирование будет сброшено к первоначальному (отчеты по уже выполненным тестам будут сохранены). Перейти к началу повторного тестированя?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        logMessage("Начало повторного тестирования");
        for (auto test : m_tests)
            test->reset();
        setCurrentTest(m_tests.first());
    }
}
