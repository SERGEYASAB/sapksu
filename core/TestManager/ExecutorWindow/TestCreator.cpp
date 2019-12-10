#include "TestCreator.h"
#include "DBConfig/DBConfig.h"
#include "TestManager/JSTest.h"
#include "TestManager/JSTestStep.h"
#include "TestManager/CppTest.h"
#include "TestManager/CppTestStep.h"
#include "ErrorClassifier/ErrorClassifier.h"
#include "ReportManager/FaultReport.h"

#define JS_TEST     0
#define CPP_TEST    1

TestCreator::TestCreator(QObject *parent) : QObject(parent)
{
    m_dataBase = DBConfig::instance()->configDatabase();
    m_funcList.append({ QStringLiteral("showMessage"), QStringLiteral("(\"text\")") });
    m_funcList.append({ QStringLiteral("delay"), QStringLiteral("(ms, \"text\")") });
    m_funcList.append({ QStringLiteral("hasVariable"), QStringLiteral("(\"alias\")") });
    m_funcList.append({ QStringLiteral("getValue"), QStringLiteral("(\"alias\")") });
    m_funcList.append({ QStringLiteral("setValue"), QStringLiteral("(\"alias\", value)") });
    m_funcList.append({ QStringLiteral("parametrAddress"), QStringLiteral("(\"alias\")") });
    m_funcList.append({ QStringLiteral("variableMetaInfo"), QStringLiteral("(\"alias\")") });
    m_funcList.append({ QStringLiteral("showQuestion"), QStringLiteral("(\"text\")") });
    m_funcList.append({ QStringLiteral("abortEvaluationQuestion"), QStringLiteral("(\"text\" = 0)") });
    m_funcList.append({ QStringLiteral("messageReport"), QStringLiteral("(\"text\")") });
    m_funcList.append({ QStringLiteral("valueReport"), QStringLiteral("(\"alias\", value, \"text\" = 0)") });
    m_funcList.append({ QStringLiteral("stepReport"), QStringLiteral("(\"bool\")") });
    m_funcList.append({ QStringLiteral("abortEvaluation"), QStringLiteral("()") });

    m_funcList.append({ QStringLiteral("readKsu735Archive"), QStringLiteral("(offset)") });
    m_funcList.append({ QStringLiteral("readModbusKsuCurrentParametrsArchive"), QStringLiteral("(offset)") });
    m_funcList.append({ QStringLiteral("readModbusKsuChangeParametrsArchive"), QStringLiteral("(offset)") });
    m_funcList.append({ QStringLiteral("readModbusKsuEventsArchive"), QStringLiteral("(offset)") });
    m_funcList.append({ QStringLiteral("readModbusKsuLaunchParametrsArchive"), QStringLiteral("(offset)") });

    m_funcList.append({ QStringLiteral("isWriteable"), QStringLiteral("(\"alias\")") });
    m_funcList.append({ QStringLiteral("manualFullTesting"), QStringLiteral("()") });
    m_funcList.append({ QStringLiteral("protocolType"), QStringLiteral("()") });
    m_funcList.append({ QStringLiteral("ksuNodeVariables"), QStringLiteral("()") });
    m_funcList.append({ QStringLiteral("processEvents"), QStringLiteral("()") });

    m_funcList.append({ QStringLiteral("readModbusPassword"), QStringLiteral("(\"level\")") });
    m_funcList.append({ QStringLiteral("writeModbusPassword"), QStringLiteral("(level, \"password\")") });
    m_funcList.append({ QStringLiteral("getTypeSU"), QStringLiteral("()") });

    m_funcList.append({ QStringLiteral("getMin"), QStringLiteral("(\"alias\")") });
    m_funcList.append({ QStringLiteral("getMax"), QStringLiteral("(\"alias\")") });

    m_funcList.append({ QStringLiteral("waitConditionMessage"), QStringLiteral("(\"alias\", value, \"text\")") });
    m_funcList.append({ QStringLiteral("waitConditionDelayMessage"), QStringLiteral("(\"alias\", value, \"text\", delay)") });
    m_funcList.append({ QStringLiteral("inputMessageInt"), QStringLiteral("(\"value\", \"text\" = 0)") });
    m_funcList.append({ QStringLiteral("getTypeKSU"), QStringLiteral("()") });
    m_funcList.append({ QStringLiteral("checkKsuLink"), QStringLiteral("()") });

}

TestCreator::~TestCreator()
{

}

TestCreator *TestCreator::instance()
{
    static TestCreator singletone;
    return &singletone;
}

QList<TestFunction> TestCreator::funcList()
{
    return m_funcList;
}

ITest *TestCreator::createTest(int testId)
{
    QString sqlString = QString("SELECT * FROM `Tests` WHERE id = %1").arg(testId);

    QSqlQuery query(m_dataBase);

    if (query.exec(sqlString) == false)
    {
        ErrorClassifier::instance()->showError(501);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + query.lastError().databaseText());
    } else {

        query.first();

        QString name = query.value("name").toString();

        QString path = QCoreApplication::applicationDirPath() +
                QDir::separator() + "DBConfig" +
                QDir::separator() + "Tests" +
                QDir::separator() +
                query.value("path").toString();

        ITest *test = Q_NULLPTR;
        switch (query.value("executeType").toInt()) {
            case JS_TEST: {
                test = new JSTest(name, path);
                break;
            }

            case CPP_TEST: {
                test = new CppTest(name, path);
                break;
            }

            default:
                break;
        }
        return test;
    }
    return nullptr;
}

void TestCreator::removeTest(const QString &name)
{
    ITest *test = nullptr;
    for (auto i : m_tests)
        if (i->name() == name)
            test = i;
    if (test) {
        m_tests.removeOne(test);
        delete test;
    }
}

QList<ITest *> TestCreator::tests()
{
    return m_tests;
}
