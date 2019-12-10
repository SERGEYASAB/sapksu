#ifndef ITEST_H
#define ITEST_H

#include "Common/PrefixHeader.h"

class ITestStep;
class IVariable;
class EngineLocalVariable;

class ITest : public QObject
{
    Q_OBJECT

public:
    explicit ITest(QObject *parent = Q_NULLPTR);
    virtual ~ITest();

    enum TestStatus {
        None,
        Init,   // инициализирован
        Deinit  // деинициализирован
    };
    Q_ENUM(TestStatus)


    QString name() const;
    QList<ITestStep *> steps() const;
    QList<IVariable *> externalVariables() const;
    QList<EngineLocalVariable *> internalVariables() const;
    ITestStep *currentStep() const;
    ITestStep *beginStep() const;
    ITestStep *endStep() const;
    TestStatus testStatus() const;
    void reset(); //Сброс статуса и текущего номера шага

    void runInitStep();
    void runDeinitStep();
    void runStep(int stepNumber);
    void runStepAndGoNext(int stepNumber);
    void runTest();
    void runTestBeforeStepNumber(int stepNumber);

    void goNextStep();



private:
    QString m_name;

    TestStatus m_testStatus = TestStatus::None;

    ITestStep *m_currentStep = nullptr;
    ITestStep *m_beginStep = nullptr;
    ITestStep *m_endStep = nullptr;

protected:

    QList<ITestStep *> m_steps;
    QList<IVariable *> m_externalVariables;
    QList<EngineLocalVariable *> m_internalVariables;

    void setName(const QString &name);
    void setSteps(const QList<ITestStep *> &steps);
    void setCurrentStep(ITestStep *currentStep);
    void setBeginStep(ITestStep *beginStep);
    void setEndStep(ITestStep *endStep);
    void setTestStatus(const TestStatus &testStatus);

    virtual bool abortFlag() const = 0;

Q_SIGNALS:
    void messageChanged(const QString &message);
    void currentStepChanged(ITestStep *currentStep);
    void currentTestDeinitAndGoNext();
};

#endif // ITEST_H
