#ifndef EXECUTORSIMPLEWINDOW_H
#define EXECUTORSIMPLEWINDOW_H

#include "Common/PrefixHeader.h"

class ITest;
class ITestStep;
class CodeEditorPlainTextEdit;
class TestExternalVariableWidget;
class TestInternalVariableWidget;

class ExecutorSimpleWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExecutorSimpleWindow(const QList<ITest *> &tests, QWidget *parent = nullptr);
    ~ExecutorSimpleWindow();

    ITest *currentTest() const;
    void setCurrentTest(ITest *currentTest);

private:

    QList<ITest *> m_tests;
    ITest *m_currentTest = nullptr; // Don't use this Directly - setCurrentTest only
    QToolBar *m_toolBar = nullptr;

    int currentStepNumber;
//    CodeEditorPlainTextEdit *m_textProgramWidget = nullptr;
//    QListWidget *m_testListWidget = nullptr;
//    QListWidget *m_stepListWidget = nullptr;
//    QTextEdit *m_logTextEdit = nullptr;

//    TestExternalVariableWidget *extVarWidget = nullptr;
//    TestExternalVariableWidget *ksuVarWidget = nullptr;
//    TestInternalVariableWidget *intVarWidget = nullptr;

    void setupToolBar();
//    void setupTextProgramWidget();
//    void setupTestListDockWidget();
//    void setupStepListDockWidget();
//    void setupLoggerOutputWidget();
//    void setupExtVarDockWidget();
//    void setupKsuVarDockWidget();
//    void setupIntVarDockWidget();
//    void fillTestListWidget();
//    void fillStepListWidget();
//    void fillVarTableWidgets();
//    void setupMainWindowSettings();

//public slots:
//    void updateTestListWidget(ITest *currentTest);
//    void updateStepListWidget(ITestStep *currentStep);
//    void updateProgramWidget(ITestStep *currentStep);
//    void readSettings();

private slots:
    bool runStep(int stepNumber);
    void runStepAndGoNext(int stepNumber);
    void goNextTest();

    void runCurrentStep();
    void runCurrentStepAndGoNext();
    void runCurrentTest();

    void logMessage(const QString &message);

//    // QWidget interface
//protected:
//    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // EXECUTORSIMPLEWINDOW_H
