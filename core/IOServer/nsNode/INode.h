#ifndef INODE_H
#define INODE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsConnection/IConnection.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsTask/IDataTask.h"
#include "IOServer/NodeError/NodeError.h"

class IDataTask;
class IVariable;

class INode : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName)
    Q_PROPERTY(bool readable READ isReadable WRITE setReadable)
    Q_PROPERTY(quint32 period READ period WRITE setPeriod)
    Q_PROPERTY(quint32 timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(quint32 timeoutOfRestore READ timeoutOfRestore WRITE setTimeoutOfRestore)
    Q_PROPERTY(quint8 numberOfRetries READ numberOfRetries WRITE setNumberOfRetries)
    Q_PROPERTY(QList<IVariable *> variables READ variables)

public:
    enum class OperationTimingType {
        Sync,
        Async
    };

    explicit INode(QObject *parent = Q_NULLPTR);
    explicit INode(QDomElement &node, QObject *parent = Q_NULLPTR);
    virtual ~INode();

    // родительский узел
    INode *parentNode() const noexcept;
    void setParentNode(INode *parentNode) noexcept;

    // название узла
    QString name() const noexcept;
    void setName(const QString &name) noexcept;

    // путь
    QString path() const noexcept;
    void setPath(const QString &path) noexcept;

    // описание
    QString description() const noexcept;
    void setDescription(const QString &description) noexcept;

    // название соединения
    QString connectionName() const noexcept;
    void setConnectionName(const QString &connectionName) noexcept;

    // разрешение на опрос
    bool isReadable() const noexcept;
    void setReadable(bool readable) noexcept;

    // период опроса
    quint32 period() const noexcept;
    void setPeriod(quint32 period) noexcept;

    // время ожидания ответа
    quint32 timeout() const noexcept;
    void setTimeout(quint32 timeout) noexcept;

    // таймаут возобновления опроса
    quint32 timeoutOfRestore() const noexcept;
    void setTimeoutOfRestore(quint32 timeoutOfRestore) noexcept;

    // количество попыток на запрос
    quint8 numberOfRetries() const noexcept;
    void setNumberOfRetries(quint8 numberOfRetries) noexcept;

    // список переменных
    QList<IVariable *> variables() const;
    virtual void appendVariable(IVariable *variable);
    void appendVariables(const QList<IVariable *> &variables);

    virtual void removeVariable(IVariable *variable);
    void removeVariables(const QList<IVariable *> &variables);

    // получить список задач, для опроса переменных узла
    inline QList<SharedPeriodicDataTaskPtr> periodicReadTasks() { return makePeriodicReadTasks(); }

    virtual bool operator<(const INode &rhs) const;

    QDomElement getConfigNode();
    void setConfigNode(QDomElement &element);

    IConnection *connection() const;
    void setConnection(IConnection *connection);

    bool hasError() const;
    NodeError lastError() const;

protected:
    // получить список переменных с доступом на чтение
    QList<IVariable *> getReadVariables() const;

    // составить список задач, для опроса переменных узла
    virtual QList<SharedPeriodicDataTaskPtr> makePeriodicReadTasks() = 0;

    //TODO: подумать над правильностью задания ошибки
    void setLastError(const NodeError &error);

signals:
    void handleTaskAsync(SharedDataTaskPtr task) const;
    QByteArray handleTaskSync(const QByteArray &request, quint32 timeout);
    void error() const;
    void noerror() const;

//    QByteArray writeDataWithSyncTask(const QByteArray &request, quint32 timeout);
//    QByteArray readDataWithSyncTask(const QByteArray &request, quint32 timeout);

protected slots:
    virtual bool parseResponse(SharedDataTaskPtr task, const QByteArray &rx) = 0;

    virtual void writeValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes) = 0;
    virtual void writeValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes) = 0;
    virtual void readValueSync(IVariable *variable) = 0;

//    virtual void createWriteDataTask(const QVariant &value) const = 0;
//    virtual void parseResponse(std::shared_ptr<IDataTask> task, const QByteArray &tx, const QByteArray &rx) const = 0;

    void setupLinkDiagnostic();

private:
    INode *_parentNode = nullptr;
    QString _name;
    QString _path;
    QString _description;
    QString _connectionName;
    bool _readable;
    quint32 _period;
    quint32 _timeout;
    quint32 _timeoutOfRestore;
    quint8 _numberOfRetries;
    QList<IVariable *> _variables;
    QDomElement configNode;

    QPointer<IConnection> m_connectionPtr;

    NodeError m_lastError;
};

#endif // INODE_H
