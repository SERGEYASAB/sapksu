#ifndef JSTEST_H
#define JSTEST_H

#include "Common/PrefixHeader.h"
#include "TestManager/ExecutorWindow/ITest.h"

class JSTestScriptEngine;

class JSTest : public ITest
{
    Q_OBJECT
public:
    explicit JSTest(const QString &name, const QString &path, QObject* parent = Q_NULLPTR);
    static QStringList funcList();
    ~JSTest();

    JSTestScriptEngine *engine() const;

protected:
    bool abortFlag() const override;

private:
    JSTestScriptEngine *m_engine;

    QDomDocument m_xmlTest;

    // Разбор xml структуры
    void traverseXmlNodes(const QDomNode &node);

    // Загрузка файла документа в xml структуру
    bool loadTestConfiguration(const QString &testPath);
};

#endif // JSTEST_H
