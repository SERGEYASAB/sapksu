#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Common/PrefixHeader.h"
#include "IDataTask.h"

class IConnection;
class INode;

class TaskManager final : public QObject
{
    Q_OBJECT

public:
    explicit TaskManager(IConnection *connection, const QList<INode *> &nodes, QThread *thread, QObject *parent = nullptr);
    ~TaskManager();

    inline IConnection *connection() const noexcept { return m_connection; }
    inline const QList<SharedPeriodicDataTaskPtr> &periodicReadTasks() const noexcept { return m_periodicReadDataTasks; }
    inline const QList<SharedDataTaskPtr> &singleTasks() const noexcept { return m_singleTasks; }

    inline QThread *workingThread() const { return m_thread; };

public slots:
    void process();
    void stop();

private slots:
    void addSingleTask(SharedDataTaskPtr taskPtr);
    QByteArray handleTaskSync(const QByteArray &request, quint32 timeout);

signals:
    void finished();

private:
    void processLoop();
    void processTask(SharedDataTaskPtr task) const;
    inline void processEvents();

    QThread *m_thread = nullptr;

    qint32 m_nextRequestDelay = 0;

    volatile bool m_stoped = true;
    IConnection *m_connection = nullptr;

    QList<SharedPeriodicDataTaskPtr> m_periodicReadDataTasks;
    QList<SharedDataTaskPtr> m_singleTasks;
};

#endif // TASKMANAGER_H
