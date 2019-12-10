#ifndef TESTCREATOR_H
#define TESTCREATOR_H

#include "Common/PrefixHeader.h"

class ITest;
class IVariable;
class EngineLocalVariable;

struct TestFunction
{
    QString name;
    QString signature;
};

class TestCreator : public QObject
{
    Q_OBJECT

public:
    static TestCreator *instance();
    QList<TestFunction> funcList();
    ITest *createTest(int testId);
    void removeTest(const QString &name);
    QList<ITest *> tests();
private:
    explicit TestCreator(QObject *parent = 0);
    ~TestCreator();

    QSqlDatabase m_dataBase;
    Q_DISABLE_COPY(TestCreator)
    QList<ITest*> m_tests;
    QList<TestFunction> m_funcList;
};

#endif // TESTCREATOR_H
