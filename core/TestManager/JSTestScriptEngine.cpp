#include "JSTestScriptEngine.h"

JSTestScriptEngine::JSTestScriptEngine(QObject *parent) :
    QScriptEngine(parent)
{

}

bool JSTestScriptEngine::abortFlag() const
{
    return m_abortFlag;
}

void JSTestScriptEngine::setAbortFlag(bool abortFlag)
{
    m_abortFlag = abortFlag;
}
