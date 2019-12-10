#ifndef CPPTESTSTEP_H
#define CPPTESTSTEP_H

#include "Common/PrefixHeader.h"
#include "TestManager/ExecutorWindow/ITestStep.h"

class CppTestStep : public ITestStep
{
    Q_OBJECT

public:
    explicit CppTestStep(QObject *parent = Q_NULLPTR);

public:
    void run() override;
};

#endif // CPPTESTSTEP_H
