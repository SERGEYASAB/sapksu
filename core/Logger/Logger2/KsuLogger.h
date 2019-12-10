#ifndef KSULOGGER_H
#define KSULOGGER_H

#include "Common/PrefixHeader.h"
#include "ILogger2.h"

class KsuLogger : public QWidget, ILogger2
{
public:
    KsuLogger(QWidget *parent = nullptr);

protected:
    virtual void log(QTime time, LoggerTaskManager2::MessageType type, const QString &text) override final;

private:
    QTableView *m_logTable = nullptr;
};

#endif // KSULOGGER_H
