#ifndef WINDOWSEVENTLOGGER_H
#define WINDOWSEVENTLOGGER_H

#include "ILogger.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define NETWORK_CATEGORY                 ((WORD)0x00000001L)
#define DATABASE_CATEGORY                ((WORD)0x00000002L)
#define UI_CATEGORY                      ((WORD)0x00000003L)
#define MSG_INVALID_COMMAND              ((DWORD)0xC0020100L)
#define MSG_BAD_FILE_CONTENTS            ((DWORD)0xC0000101L)
#define MSG_RETRIES                      ((DWORD)0x80000102L)
#define MSG_COMPUTE_CONVERSION           ((DWORD)0x40000103L)
#define QUARTS_UNITS                     ((DWORD)0x00001000L)
#define GALLONS_UNITS                    ((DWORD)0x00001001L)

class WindowsEventLogger : public ILogger
{
    Q_OBJECT

public:
    explicit WindowsEventLogger(const QString &configFileName = QString(),
                                const QString &configGroupName = QString());

public slots:
    void log(LoggerTaskManager::LogLevel level, const QString &text);
};

#endif // WINDOWSEVENTLOGGER_H
