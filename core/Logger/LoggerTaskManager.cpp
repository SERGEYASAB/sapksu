#include "LoggerTaskManager.h"
#include <iostream>
#include "Common/ThreadWorkingTemplates.h"

LoggerTaskManager::LoggerTaskManager()
{
    setGlobalFilter(Info | Warning | Critical | Debug);
}

LoggerTaskManager::~LoggerTaskManager()
{

}

LoggerTaskManager *LoggerTaskManager::getInstance()
{
    static LoggerTaskManager singletone;
    return &singletone;
}

void LoggerTaskManager::log(const QString &text, LoggerTaskManager::LogLevel level)
{
    if (globalFilter & level)
    {
        logBuffer.append({ level, QTime::currentTime(), text });
        emit timeToLog(logBuffer.last());
    }

#ifdef QT_DEBUG
    QTextStream(stdout) << text << endl;
#endif
}

void LoggerTaskManager::setGlobalFilter(LoggerTaskManager::LogLevels filter)
{
    globalFilter = filter;
}

QVector<LoggerTaskManager::LogLine> &LoggerTaskManager::getBuffer()
{
    return logBuffer;
}

void appMessageOutputHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    QThread *mainThread = QApplication::instance()->thread();

    switch (type) {
        case QtDebugMsg:
            executeInThread(mainThread, [msg]() {
                LoggerTaskManager::getInstance()->log(msg, LoggerTaskManager::Debug);
            });
            break;

        case QtWarningMsg:
            executeInThread(mainThread, [msg]() {
                LoggerTaskManager::getInstance()->log(msg, LoggerTaskManager::Warning);
            });
            break;

        case QtCriticalMsg:
            executeInThread(mainThread, [msg]() {
                LoggerTaskManager::getInstance()->log(msg, LoggerTaskManager::Critical);
            });
            break;

        case QtInfoMsg:
            executeInThread(mainThread, [msg]() {
                LoggerTaskManager::getInstance()->log(msg, LoggerTaskManager::Info);
            });
            break;

        case QtFatalMsg:
            executeInThread(mainThread, [msg]() {
                LoggerTaskManager::getInstance()->log(msg, LoggerTaskManager::Critical);
                abort();
            });
            break;
    }

}
