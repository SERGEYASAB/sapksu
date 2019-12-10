#ifndef JSTESTSTEP_H
#define JSTESTSTEP_H

#include "Common/PrefixHeader.h"
#include "TestManager/ExecutorWindow/ITestStep.h"

class JSTestScriptEngine;

class JSTestStep : public ITestStep
{
    Q_OBJECT
public:
    explicit JSTestStep(JSTestScriptEngine *engine, QObject *parent = Q_NULLPTR);

public:
    void run() override;

private:
    JSTestScriptEngine *m_engine;
};

#endif // JSTESTSTEP_H
