#ifndef CPPTEST_H
#define CPPTEST_H

#include "Common/PrefixHeader.h"
#include "TestManager/ExecutorWindow/ITest.h"

class CppTest : public ITest
{
public:
    explicit CppTest(const QString &name, const QString &path, QObject* parent = Q_NULLPTR);
    ~CppTest();

protected:
    bool abortFlag() const override;
};

#endif // CPPTEST_H
