#ifndef EXECUTORWINDOW_H
#define EXECUTORWINDOW_H

#include "Common/PrefixHeader.h"

class ITest;
class ITestStep;
class CodeEditorPlainTextEdit;
class TestExternalVariableWidget;
class TestInternalVariableWidget;
class KsuLogWidget;
class ReportConfigure;

class ExecutorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExecutorWindow(const QList<ITest *> &tests, QWidget *parent = nullptr);
    ~ExecutorWindow();

    ITest *currentTest() const;
    void setCurrentTest(ITest *currentTest);

private:

    QList<ITest *> m_tests;
    ITest *m_currentTest = nullptr; // Don't use this Directly - setCurrentTest only
    QToolBar *m_toolBar = nullptr;
    CodeEditorPlainTextEdit *m_textProgramWidget = nullptr;
    QListWidget *m_testListWidget = nullptr;
    QListWidget *m_stepListWidget = nullptr;
    QTextEdit *m_logTextEdit = nullptr;
    QLabel *m_pauseStateLabel = nullptr;
    QAction *m_pauseStateLabelAction = nullptr;
    QSpinBox *m_stepNumberSpinBox = nullptr;
    QDockWidget *m_logDockWidget = nullptr;
    QDockWidget *m_logDock = nullptr;
    KsuLogWidget *m_ksuLogWidget = nullptr;
    ReportConfigure *m_reportConfig = nullptr;

    TestExternalVariableWidget *extVarWidget = nullptr;
    TestExternalVariableWidget *ksuVarWidget = nullptr;
    TestInternalVariableWidget *intVarWidget = nullptr;

    QDateTime m_beginDate = QDateTime::currentDateTime();

    bool m_testFinished = false;

    void setupToolBar();
    void setupLogDockWidget();
    void setupTextProgramWidget();
    void setupTestListDockWidget();
    void setupStepListDockWidget();
    void setupLoggerOutputWidget();
    void setupExtVarDockWidget();
    void setupKsuVarDockWidget();
    void setupIntVarDockWidget();
    void fillTestListWidget();
    void fillStepListWidget();
    void fillVarTableWidgets();
    void setupMainWindowSettings();
    bool checkPressContinue();

    // DEBUG:
    void sendPause();
    void sendContinue();
    // END DEBUG

public slots:
    void updateTestListWidget(ITest *currentTest);
    void updateStepListWidget(ITestStep *currentStep);
    void updateProgramWidget(ITestStep *currentStep);
    void readSettings();

private slots:
    bool runStep(int stepNumber);
    void runStepAndGoNext(int stepNumber);
    void goNextTest();

    void runCurrentStep();
    void runCurrentStepAndGoNext();
    void runCurrentTest();
    void runAllTests();
    void runCurrenTestBeforeStepNumber(int stepNumber);

    void executeTest(ITest *test);
    void logMessage(const QString &message);
    void showReportWindow();
    void previewReportWindow();
    void printToPdf();

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
    void restartTestingPrepare();
};

#endif // EXECUTORWINDOW_H
