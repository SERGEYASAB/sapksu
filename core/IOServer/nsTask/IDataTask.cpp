#include "IDataTask.h"
#include "IOServer/nsNode/INode.h"

IDataTask::IDataTask(const QByteArray &request, void *userData) noexcept :
    IDataTask(request, 0, userData)
{  

}

IDataTask::IDataTask(const QByteArray &request, quint32 timeout, void *userData) noexcept :
    QObject(), m_request(request), m_timeout(timeout), m_userData(userData)
{
    qMetaTypeId<SharedDataTaskPtr>();
}

void IDataTask::emitReceivedResponse(const SharedDataTaskPtr &task, const QByteArray &response) const
{
    emit receivedResponse(task, response, QPrivateSignal());
}


ReadDataTask::ReadDataTask(const QByteArray &request, void *userData) noexcept :
    IDataTask(request, userData)
{

}

ReadDataTask::ReadDataTask(const QByteArray &request, quint32 timeout, void *userData) noexcept :
    IDataTask(request, timeout, userData)
{

}

void ReadDataTask::response(const SharedDataTaskPtr &task, const QByteArray &response) const
{
    emitReceivedResponse(task, response);
}


PeriodicReadDataTask::PeriodicReadDataTask(const QByteArray &request, void *userData) noexcept :
    PeriodicReadDataTask(request, 0, 0, userData)
{

}

PeriodicReadDataTask::PeriodicReadDataTask(const QByteArray &request, quint32 timeout, void *userData) noexcept :
    PeriodicReadDataTask(request, timeout, 0, userData)
{

}

PeriodicReadDataTask::PeriodicReadDataTask(const QByteArray &request, quint32 timeout, quint32 period, void *userData) noexcept :
    ReadDataTask(request, timeout, userData), m_period(period)
{

}

WriteDataTask::WriteDataTask(IVariable *variable, const QVariant &value, const QByteArray &request, quint32 timeout, void *userData) noexcept :
    IDataTask(request, timeout, userData)
{
    m_writedValues[variable] = value;
}

WriteDataTask::WriteDataTask(const QMap<IVariable *, QVariant> &writedValues, const QByteArray &request, quint32 timeout, void *userData) noexcept :
    IDataTask(request, timeout, userData), m_writedValues(writedValues)
{

}

void WriteDataTask::response(const SharedDataTaskPtr &task, const QByteArray &response) const
{
    emitReceivedResponse(task, response);
}
