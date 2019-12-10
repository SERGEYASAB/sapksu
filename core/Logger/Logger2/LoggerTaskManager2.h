#ifndef LoggerTaskManager2_H
#define LoggerTaskManager2_H

#include <QObject>
#include <QTime>
#include <QtWidgets>

class LoggerTaskManager2 : public QObject
{
    Q_OBJECT

public:

    enum MessageType {
        Info     = 0,
        Rx          = 1,
        Tx          = 2,
        Error       = 4
    };

    Q_DECLARE_FLAGS(MessageTypes, MessageType)
    Q_FLAG(MessageTypes)

    static LoggerTaskManager2 *getInstance();

public slots:
    void log(const QString &text, MessageType type);

signals:
    void timeToLog(QTime time, MessageType type, const QString &text);

private:
    explicit LoggerTaskManager2();
    ~LoggerTaskManager2();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(LoggerTaskManager2::MessageTypes)

#endif // LoggerTaskManager2_H
