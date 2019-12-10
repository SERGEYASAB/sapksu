#ifndef LOGGERTASKMANAGER_H
#define LOGGERTASKMANAGER_H

#include <QObject>
#include <QTime>
#include <QtWidgets>

void appMessageOutputHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

class LoggerTaskManager : public QObject
{
    Q_OBJECT

public:

    enum LogLevel {
        Empty       = 0,
        Info        = 1,
        Warning     = 2,
        Critical    = 4,
        Debug       = 8
    };

    Q_DECLARE_FLAGS(LogLevels, LogLevel)
    Q_FLAG(LogLevels)


    struct LogLine
    {
        LogLevel level;
        QTime timeStamp;
        QString text;
    };

    static LoggerTaskManager *getInstance();

public slots:
    void log(const QString &text, LoggerTaskManager::LogLevel level);
    void setGlobalFilter(LoggerTaskManager::LogLevels filter);
    QVector<LogLine> &getBuffer();

signals:
    void timeToLog(LoggerTaskManager::LogLine &);

private:
    explicit LoggerTaskManager();
    ~LoggerTaskManager();

    LogLevels globalFilter;
    QVector<LogLine> logBuffer;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(LoggerTaskManager::LogLevels)

#endif // LOGGERTASKMANAGER_H
