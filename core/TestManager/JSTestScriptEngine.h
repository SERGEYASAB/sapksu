#ifndef JSTESTSCRIPTENGINE_H
#define JSTESTSCRIPTENGINE_H

#include "Common/PrefixHeader.h"

class JSTestScriptEngine : public QScriptEngine
{
    Q_OBJECT
    Q_PROPERTY(bool abortFlag READ abortFlag WRITE setAbortFlag)

public:
    explicit JSTestScriptEngine(QObject *parent = nullptr);

    bool abortFlag() const;
    void setAbortFlag(bool abortFlag);

private:
    bool m_abortFlag = false;
};

#endif // JSTESTSCRIPTENGINE_H
