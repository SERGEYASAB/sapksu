#include "LoggerTaskManager2.h"

LoggerTaskManager2::LoggerTaskManager2()
{
    QDir logDir = QApplication::applicationDirPath() + "/Logs";
    if(!logDir.exists())
    {
        logDir.cdUp();
        logDir.mkdir("Logs");
    }
}

LoggerTaskManager2::~LoggerTaskManager2()
{

}

LoggerTaskManager2 *LoggerTaskManager2::getInstance()
{
    static LoggerTaskManager2 singletone;
    return &singletone;
}

void LoggerTaskManager2::log(const QString &text, LoggerTaskManager2::MessageType type)
{
    emit timeToLog(QTime::currentTime(), type, text);
}
