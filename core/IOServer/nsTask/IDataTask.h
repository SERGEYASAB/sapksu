#ifndef IDATATASK_H
#define IDATATASK_H

#include "Common/PrefixHeader.h"

class INode;
class IVariable;

class IDataTask;
class PeriodicReadDataTask;

typedef SSPtr<IDataTask> SharedDataTaskPtr;
typedef SUPtr<IDataTask> UniqueDataTaskPtr;

typedef SSPtr<PeriodicReadDataTask> SharedPeriodicDataTaskPtr;


class IDataTask : public QObject
{
    Q_OBJECT

public:
    explicit IDataTask(const QByteArray &request, void *userData = nullptr) noexcept;
    explicit IDataTask(const QByteArray &request, quint32 timeout, void *userData = nullptr) noexcept;
    virtual ~IDataTask() = default;

    inline const QByteArray &request() const noexcept { return m_request; }

    virtual void response(const SharedDataTaskPtr &task, const QByteArray &response) const = 0;
    inline void *userData() { return m_userData; }

    inline quint32 timeout() const { return m_timeout; }
    inline void setTimeout(quint32 timeout) { m_timeout = timeout; }

signals:
    void receivedResponse(SharedDataTaskPtr task, QByteArray response, QPrivateSignal) const;

protected:
    void emitReceivedResponse(const SharedDataTaskPtr &task, const QByteArray &response) const;

private:
    QByteArray m_request;
    quint32 m_timeout = 0;
    void *m_userData = nullptr;
};


class ReadDataTask : public IDataTask
{
public:
    explicit ReadDataTask(const QByteArray &request, void *userData = nullptr) noexcept;
    explicit ReadDataTask(const QByteArray &request, quint32 timeout, void *userData = nullptr) noexcept;

public:
    void response(const SharedDataTaskPtr &task, const QByteArray &response) const override;
};


class PeriodicReadDataTask : public ReadDataTask
{
public:
    explicit PeriodicReadDataTask(const QByteArray &request, void *userData = nullptr) noexcept;
    explicit PeriodicReadDataTask(const QByteArray &request, quint32 timeout, void *userData = nullptr) noexcept;
    explicit PeriodicReadDataTask(const QByteArray &request, quint32 timeout, quint32 period, void *userData = nullptr) noexcept;

    inline quint32 period() const noexcept { return m_period; }
    void setPeriod(quint32 period) noexcept { m_period = period; }

    qint64 lastRequestTime() const noexcept { return m_lastRequestTime; }
    void setLastRequestTime(qint64 lastRequestTime) noexcept { m_lastRequestTime = lastRequestTime; }

private:
    quint32 m_period = 0;
    qint64 m_lastRequestTime = 0;
};


class WriteDataTask : public IDataTask
{
public:
    WriteDataTask(IVariable *variable, const QVariant &value, const QByteArray &request, quint32 timeout = 0, void *userData = nullptr) noexcept;
    WriteDataTask(const QMap<IVariable *, QVariant> &writedValues, const QByteArray &request, quint32 timeout = 0, void *userData = nullptr) noexcept;

    void response(const SharedDataTaskPtr &task, const QByteArray &response) const override;

    inline const QMap<IVariable *, QVariant> &writedValues() const noexcept { return m_writedValues; }

private:
    QMap<IVariable *, QVariant> m_writedValues;
};

Q_DECLARE_METATYPE(SharedDataTaskPtr)
Q_DECLARE_METATYPE(SharedPeriodicDataTaskPtr)

#endif // IDATATASK_H
