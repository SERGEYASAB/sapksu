#include "TaskManager.h"

#include "Common/PrefixHeader.h"
#include "IOServer/nsTask/IDataTask.h"
#include "IOServer/nsConnection/IConnection.h"
#include "IOServer/nsNode/INode.h"

#include <QThread>
#include <chrono>

TaskManager::TaskManager(IConnection *connection, const QList<INode *> &nodes, QThread *thread, QObject *parent) :
    QObject(parent), m_thread(thread), m_stoped(true)
{
    m_connection = connection;
    m_nextRequestDelay = connection->nextRequestDelay();

    for (auto node : nodes) {
        connect(node, &INode::handleTaskAsync, this, &TaskManager::addSingleTask);
        connect(node, &INode::handleTaskSync, this, &TaskManager::handleTaskSync, Qt::BlockingQueuedConnection);

        m_periodicReadDataTasks.append(node->periodicReadTasks());
    }
}

TaskManager::~TaskManager()
{

}

void TaskManager::process()
{
    m_stoped = false;
    m_connection->open();

    processLoop();

    m_connection->close();
    emit finished();
}

void TaskManager::stop()
{
    if (!m_stoped) {
        m_stoped = true;
    }
}

void TaskManager::addSingleTask(SharedDataTaskPtr taskPtr)
{
    m_singleTasks.append(taskPtr);
}

QByteArray TaskManager::handleTaskSync(const QByteArray &request, quint32 timeout)
{
    return m_connection->sendData(request, timeout);
}

void TaskManager::processLoop()
{
    forever {
        processEvents();
        if (m_stoped)
            return;

        // Вставка задачи на запись
        QList<SharedDataTaskPtr> *singlesTasks = new QList<SharedDataTaskPtr>(m_singleTasks);
        for (auto &singleTask : *singlesTasks) {
            processEvents();
            if (m_stoped)
                return;

            QByteArray response = m_connection->sendData(singleTask->request(), singleTask->timeout());
            singleTask->response(singleTask, response);
            m_singleTasks.removeOne(singleTask);
        } // конец вставки

        singlesTasks->clear();
        delete singlesTasks;
        singlesTasks = nullptr;


        for (auto &periodicTask : m_periodicReadDataTasks) {
            processEvents();
            if (m_stoped)
                return;

            //чтение значений
            if (QDateTime::currentMSecsSinceEpoch() >= periodicTask->period() + periodicTask->lastRequestTime()) {
                QByteArray response = m_connection->sendData(periodicTask->request(), periodicTask->timeout());

#ifdef IODEBUG
//                qDebug() << "REQUEST: " << periodicTask->request().toHex();
//                qDebug() << "RESPONSE: " << response.toHex();
#endif

                periodicTask->setLastRequestTime(QDateTime::currentMSecsSinceEpoch());
                emit periodicTask->response(periodicTask, response);
            }
        }
    }
}

void TaskManager::processTask(SharedDataTaskPtr task) const
{
    QByteArray response = m_connection->sendData(task->request(), task->timeout());
    task->response(task, response);
}

void TaskManager::processEvents()
{
    auto start = std::chrono::system_clock::now();
    QApplication::processEvents();
    auto end = std::chrono::system_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    auto msec = m_nextRequestDelay - static_cast<qint64>(elapsed);
    if (msec > 0 && m_nextRequestDelay != 0) {
        QThread::msleep(msec);
    }
}

