#include "ITest.h"
#include "ITestStep.h"
#include "IOServer/IOServer.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "DBConfig/DBConfig.h"
#include "Authentication/SystemUser.h"
#include "ErrorClassifier/ErrorClassifier.h"

ITest::ITest(QObject *parent)
{
    Q_UNUSED(parent)

    setTestStatus(TestStatus::None);
}

ITest::~ITest()
{

}

QString ITest::name() const
{
    return m_name;
}

void ITest::setName(const QString &name)
{
    m_name = name;
}

QList<ITestStep *> ITest::steps() const
{
    return m_steps;
}

ITestStep *ITest::currentStep() const
{
    return m_currentStep;
}

ITestStep *ITest::beginStep() const
{
    return m_beginStep;
}

ITestStep *ITest::endStep() const
{
    return m_endStep;
}

ITest::TestStatus ITest::testStatus() const
{
    return m_testStatus;
}

void ITest::setTestStatus(const TestStatus &testStatus)
{
    m_testStatus = testStatus;
}

void ITest::reset()
{
    setTestStatus(ITest::None);
    setCurrentStep(m_steps.first());
}

void ITest::setEndStep(ITestStep *endStep)
{
    m_endStep = endStep;
}

void ITest::runInitStep()
{
    if (testStatus() == TestStatus::Init) {
        emit messageChanged(tr("Текущий тест уже был инициализирован"));
        return;
    }

    setCurrentStep(m_beginStep);

#ifdef SAP_KSU
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, result, stepNumber) "
                                "VALUES('init', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                        .arg(SystemUser::instance()->user())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                        .arg("Инициализация")
                        .arg(IOServer::instance()->getTestHistoryReport()->testKitName())
                        .arg(IOServer::instance()->getTestHistoryReport()->testName())
                        .arg("init")
                        .arg(true)
                        .arg(IOServer::instance()->getTestHistoryReport()->stepNumber());

    QSqlQuery query(db);
    db.transaction();
    query.exec(sqlString);
    db.commit();
#endif
#ifdef SAP_SU
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                "(function, user, ksuName, softwareVersion, kitName, testName, stepName, result, stepNumber, message) "
                                "VALUES('init', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                        .arg(SystemUser::instance()->user())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                        .arg(IOServer::instance()->getTestHistoryReport()->testKitName())
                        .arg(IOServer::instance()->getTestHistoryReport()->testName())
                        .arg("init")
                        .arg(true)
                        .arg(IOServer::instance()->getTestHistoryReport()->stepNumber())
                        .arg("Номер протокола: " + IOServer::instance()->getTestHistoryReport()->protokolNumberLineEdit() +
                             "Тип СУ: " + IOServer::instance()->getTestHistoryReport()->typeSULineEdit() +
                             "Заводской номер СУ: " + IOServer::instance()->getTestHistoryReport()->serialNumberSULineEdit() +
                             "Год выпуска СУ: " + IOServer::instance()->getTestHistoryReport()->issueYearSULineEdit() +
                             "Инвентарный номер СУ: " + IOServer::instance()->getTestHistoryReport()->invNumberSULineEdit() +
                             "Тип КСУ: " + IOServer::instance()->getTestHistoryReport()->typeKSULineEdit() +
                             "Заводской номер КСУ: " + IOServer::instance()->getTestHistoryReport()->serialNumberKSULineEdit() +
                             "Версия ПО КСУ: " + IOServer::instance()->getTestHistoryReport()->versionPOKSULineEdit() +
                             "Вид ремонта СУ: " + IOServer::instance()->getTestHistoryReport()->kindOfRepairsLineEdit());

    QSqlQuery query(db);
    db.transaction();
    query.exec(sqlString);
    db.commit();

#endif
#ifdef MECHOS
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                "(function, user, ksuName, softwareVersion, kitName, testName, stepName, result, stepNumber, message) "
                                "VALUES('init', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                        .arg(SystemUser::instance()->user())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                        .arg(IOServer::instance()->getTestHistoryReport()->testKitName())
                        .arg(IOServer::instance()->getTestHistoryReport()->testName())
                        .arg("init")
                        .arg(true)
                        .arg(IOServer::instance()->getTestHistoryReport()->stepNumber())
                        .arg("Номер протокола: " + IOServer::instance()->getTestHistoryReport()->protokolNumberLineEdit() +
                             "Тип СУ: " + IOServer::instance()->getTestHistoryReport()->typeSULineEdit() +
                             "Заводской номер СУ: " + IOServer::instance()->getTestHistoryReport()->serialNumberSULineEdit() +
                             "Год выпуска СУ: " + IOServer::instance()->getTestHistoryReport()->issueYearSULineEdit() +
                             "Инвентарный номер СУ: " + IOServer::instance()->getTestHistoryReport()->invNumberSULineEdit() +
                             "Тип КСУ: " + IOServer::instance()->getTestHistoryReport()->typeKSULineEdit() +
                             "Заводской номер КСУ: " + IOServer::instance()->getTestHistoryReport()->serialNumberKSULineEdit() +
                             "Версия ПО КСУ: " + IOServer::instance()->getTestHistoryReport()->versionPOKSULineEdit() +
                             "Вид ремонта СУ: " + IOServer::instance()->getTestHistoryReport()->kindOfRepairsLineEdit());

    QSqlQuery query(db);
    db.transaction();
    query.exec(sqlString);
    db.commit();

#endif

    m_beginStep->run();

    setTestStatus(TestStatus::Init);
}

void ITest::runDeinitStep()
{
    if (testStatus() == TestStatus::Deinit) {
        emit messageChanged(tr("Текущий тест уже был деинициализирован"));
        return;
    }

    if (testStatus() == TestStatus::None) {
        emit messageChanged(tr("Текущий тест еще не был инициализирован"));
        return;
    }

    setCurrentStep(m_endStep);
    m_endStep->run();

    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, result, stepNumber) "
                                "VALUES('deinit', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                        .arg(SystemUser::instance()->user())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                        .arg("Деинициализация")
                        .arg(IOServer::instance()->getTestHistoryReport()->testKitName())
                        .arg(IOServer::instance()->getTestHistoryReport()->testName())
                        .arg("deinit")
                        .arg(true)
                        .arg(IOServer::instance()->getTestHistoryReport()->stepNumber());

    QSqlQuery query(db);
    db.transaction();
    query.exec(sqlString);
    db.commit();

    setTestStatus(TestStatus::Deinit);
}

void ITest::runStep(int stepNumber)
{
    if (!IOServer::instance())
        return;

    if (abortFlag()) {
        runDeinitStep();
        return;
    }

    IOServer::instance()->getTestHistoryReport()->setTestName(name());
    IOServer::instance()->getTestHistoryReport()->setStepNumber(QString::number(stepNumber));

    if (steps().isEmpty())
        return;

    if (steps().count() < stepNumber)
        return;

    if (steps().at(stepNumber) == beginStep()) {
        runInitStep();
    } else if (steps().at(stepNumber) == endStep()) {
        runDeinitStep();
    } else {

        ITestStep *executedStep = steps().at(stepNumber);
        setCurrentStep(executedStep);
        executedStep->run();
    }
}

void ITest::goNextStep()
{
    int currentStepIndex = steps().indexOf(currentStep());

    if (steps().count() != currentStepIndex + 1) {
        setCurrentStep(steps().at(currentStepIndex + 1));

    } else {
        emit currentTestDeinitAndGoNext();
    }
}

QList<IVariable *> ITest::externalVariables() const
{
    return m_externalVariables;
}

QList<EngineLocalVariable *> ITest::internalVariables() const
{
    return m_internalVariables;
}

void ITest::runStepAndGoNext(int stepNumber)
{
    runStep(stepNumber);
    goNextStep();
}

void ITest::runTest()
{
    int index = m_steps.indexOf(currentStep()) < 0 ? 0 : m_steps.indexOf(currentStep());
    for (int a = index; a < m_steps.count(); a++) {

        if (IOServer::instance()->getPauseTestStopButton())
            break;

        if (testStatus() != ITest::Deinit)
            runStepAndGoNext(a);
    }
}

void ITest::runTestBeforeStepNumber(int stepNumber)
{
    int index = m_steps.indexOf(currentStep()) < 0 ? 0 : m_steps.indexOf(currentStep());

    if (index >= stepNumber || stepNumber > m_steps.count()) {
        ErrorClassifier::instance()->showError(42);
        return;
    }


    for (int a = index; a < stepNumber; a++) {

        if (IOServer::instance()->getPauseTestStopButton())
            break;
        if (testStatus() != ITest::Deinit)
            runStepAndGoNext(a);
    }
}

void ITest::setBeginStep(ITestStep *beginStep)
{
    m_beginStep = beginStep;
}

void ITest::setCurrentStep(ITestStep *currentStep)
{
    if (m_currentStep == currentStep)
        return;
    if (m_currentStep != nullptr)
        disconnect(m_currentStep, &ITestStep::messageChanged, this, &ITest::messageChanged);

    m_currentStep = currentStep;
    connect(currentStep, &ITestStep::messageChanged, this, &ITest::messageChanged);
    emit currentStepChanged(currentStep);
}

void ITest::setSteps(const QList<ITestStep *> &steps)
{
    if (m_steps == steps)
        return;

    m_steps = steps;

    if (!steps.isEmpty())
        setCurrentStep(steps.first());
    else
        setCurrentStep(nullptr);
}

