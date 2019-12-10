#include "JSTestStep.h"
#include "IOServer/IOServer.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "JSTestScriptEngine.h"
#include "ReportManager/FaultReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

JSTestStep::JSTestStep(JSTestScriptEngine *engine, QObject *parent)
    : ITestStep(parent),
    m_engine(engine)
{
}

void JSTestStep::run()
{
    if (!IOServer::instance())
        return;

//    if (m_engine->abortFlag())
//        return;

    IOServer::instance()->getTestHistoryReport()->setStepName(name());

    m_engine->collectGarbage();
    m_engine->clearExceptions();

    QScriptValue value;

    if (!m_engine->canEvaluate(textProgram())) {
        setStatus(StepStatus::Exception);
        setMessage(tr("Программа не может быть выполнена"));
        return;
    }

    setStatus(StepStatus::Executing);

    m_engine->pushContext();
    value = m_engine->evaluate(textProgram());

#ifdef QT_DEBUG
    if (m_engine->hasUncaughtException()) {
        qDebug() << "hasUncaughtException" << m_engine->uncaughtException().toString() << m_engine->uncaughtExceptionLineNumber() << m_engine->uncaughtExceptionBacktrace();

        ErrorClassifier::instance()->setErrorDescription(503, "Необработанное исключение в скрипте: строка "+ QString::number(m_engine->uncaughtExceptionLineNumber()) + "Проверьте корректность кода скрипта, либо о+братитесь к разработчикам.");
        ErrorClassifier::instance()->showError(503);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(503));
    }
#endif


    m_engine->popContext();


    if (m_engine->hasUncaughtException()) {
        value = m_engine->uncaughtException();
        setStatus(StepStatus::Exception);
        setMessage(m_engine->uncaughtException().toString());
        return;
    }

    //StepStatus::Failure

    setStatus(StepStatus::Success);
    setMessage(QString(tr("Тест %1 успешно выполнен")).arg(name()));
}
