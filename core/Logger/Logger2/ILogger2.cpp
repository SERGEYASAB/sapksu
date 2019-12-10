#include "ILogger2.h"

ILogger2::ILogger2()
{
    //connect(LoggerTaskManager2::getInstance(), &LoggerTaskManager2::timeToLog, this, ILogger2::log);
}

void ILogger2::log(QTime time, LoggerTaskManager2::MessageType type, const QString &text)
{
    qDebug()<<time.toString()<<": "<<text;;
}
