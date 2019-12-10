#ifndef ILOGGER2_H
#define ILOGGER2_H

#include "Common/PrefixHeader.h"
#include "Logger/Logger2/LoggerTaskManager2.h"

class ILogger2 : public QObject
{
    Q_OBJECT
public:
    ILogger2();

protected:
    virtual void log(QTime time, LoggerTaskManager2::MessageType type, const QString &text);
};

#endif // ILOGGER2_H
