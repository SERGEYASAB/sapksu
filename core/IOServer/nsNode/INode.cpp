#include "INode.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsTask/IDataTask.h"
#include "IOServer/IOServer.h"
#include "StartWindow/SystemTrayIcon.h"
#include "ReportManager/FaultReport.h"
#include <QDateTime>
#include <QDebug>

INode::INode(QObject *parent) : QObject(parent), m_lastError(NodeError::noError())
{
    setupLinkDiagnostic();
}

INode::INode(QDomElement &node, QObject *parent) : QObject(parent), m_lastError(NodeError::noError())
{
    configNode = node;
    setupLinkDiagnostic();
}

INode::~INode()
{
}

INode *INode::parentNode() const noexcept
{
    return _parentNode;
}

void INode::setParentNode(INode *parentNode) noexcept
{
    if (parentNode == this)
        return;
    _parentNode = parentNode;
}

QString INode::name() const noexcept
{
    return _name;
}

void INode::setName(const QString &name) noexcept
{
    _name = name;
}

QString INode::description() const noexcept
{
    return _description;
}

void INode::setDescription(const QString &description) noexcept
{
    _description = description;
}

QString INode::connectionName() const noexcept
{
    return _connectionName;
}

void INode::setConnectionName(const QString &connectionName) noexcept
{
    _connectionName = connectionName;
}

bool INode::isReadable() const noexcept
{
    return _readable;
}

void INode::setReadable(bool readable) noexcept
{
    _readable = readable;
}

quint32 INode::period() const noexcept
{
    return _period;
}

void INode::setPeriod(quint32 period) noexcept
{
    _period = period;
}

QList<IVariable *> INode::variables() const
{
    return _variables;
}

bool INode::operator<(const INode &rhs) const
{
    return name() < rhs.name();
}

void INode::appendVariable(IVariable *variable)
{
    if (!variable)
        return;

    _variables.append(variable);
    //    connect(variable, SIGNAL(writeValueSync(IVariable *, QVariant)), this, SLOT(writeValueAsync(IVariable *, QVariant)));
    connect(variable, &IVariable::writeDeviceValueSync, this, &INode::writeValueSync);
    connect(variable, &IVariable::writeDeviceValueAsync, this, &INode::writeValueAsync);
    connect(variable, &IVariable::readValueSync, this, &INode::readValueSync);
}

void INode::appendVariables(const QList<IVariable *> &variables)
{
    for (IVariable *variable : variables) {
        appendVariable(variable);
    }
}

void INode::removeVariable(IVariable *variable)
{
    _variables.removeOne(variable);

    disconnect(variable, &IVariable::writeDeviceValueSync, this, &INode::writeValueSync);
    disconnect(variable, &IVariable::writeDeviceValueAsync, this, &INode::writeValueAsync);
    disconnect(variable, &IVariable::readValueSync, this, &INode::readValueSync);
}

void INode::removeVariables(const QList<IVariable *> &variables)
{
    for (IVariable *variable : variables) {
        removeVariable(variable);
    }
}

QDomElement INode::getConfigNode()
{
    return configNode;
}

void INode::setConfigNode(QDomElement &element)
{
    configNode = element;
}

QList<IVariable *> INode::getReadVariables() const
{
    QList<IVariable *> readVariables;

    for (auto i : _variables) {
        if (i->isReadable()) {
            readVariables.append(i);
        }
    }

    return readVariables;
}

void INode::setLastError(const NodeError &e)
{
    if (m_lastError != e) {
        m_lastError = e;
        if (m_lastError != NodeError::noError()) {
            emit error();
        } else {
            emit noerror();
        }
    }
}

void INode::setupLinkDiagnostic()
{
    connect(this, &INode::error, [this]()
    {
        QSystemTrayIcon *systemTrayIcon = SystemTrayIcon::instance()->trayIcon();

        if (!systemTrayIcon)
            return;

        if (hasError()) {

            // TODO: Игнорируем всплывающую ошибку о недопустимом адресе (пока так, времянака)
            if (name() == "UZ1" && m_lastError.description() == "Недопустимый адрес") {
                return;
            }

            systemTrayIcon->show();
            systemTrayIcon->showMessage(QString("Ошибка узла %1").arg(name()),
                                        m_lastError.description(),
                                        QSystemTrayIcon::Warning);

            FaultReport::instance()->write(QString("Ошибка узла %1").arg(name()) + m_lastError.description());


            if (name() == "KsuNode") {

                systemTrayIcon->setIcon(QIcon(":/Images/bulbDigitalInputDisabled.png"));
                systemTrayIcon->setToolTip("Связь: Нет");
                systemTrayIcon->show();

            } else if (name() == "A7" || name() == "A8" || name() == "A9" || name() == "A10") {
                SystemTrayIcon::instance()->IPCconAction()->setIcon(QIcon(":/Images/bulbDigitalInputDisabled.png"));
            } else if (name() == "Circutor1") {
                SystemTrayIcon::instance()->circutor1Action()->setIcon(QIcon(":/Images/bulbDigitalInputDisabled.png"));
            } else if (name() == "Circutor2") {
                SystemTrayIcon::instance()->circutor2Action()->setIcon(QIcon(":/Images/bulbDigitalInputDisabled.png"));
            } else if (name() == "UZ1") {
                SystemTrayIcon::instance()->uz1Action()->setIcon(QIcon(":/Images/bulbDigitalInputDisabled.png"));
            } else if (name() == "MKTN") {
                SystemTrayIcon::instance()->MKTNAction()->setIcon(QIcon(":/Images/bulbDigitalInputDisabled.png"));
            }
        }
    });

    connect(this, &INode::noerror, [this]()
    {
        QSystemTrayIcon *systemTrayIcon = SystemTrayIcon::instance()->trayIcon();

        if (!systemTrayIcon)
            return;

        if (!hasError()) {

            if (name() == "KsuNode") {

                systemTrayIcon->setIcon(QIcon(":/Images/bulbDigitalInputEnabled.png"));
                systemTrayIcon->setToolTip("Связь: Норма");
                systemTrayIcon->show();

            } else if (name() == "A7" || name() == "A8" || name() == "A9" || name() == "A10") {
                SystemTrayIcon::instance()->IPCconAction()->setIcon(QIcon(":/Images/bulbDigitalInputEnabled.png"));
            } else if (name() == "Circutor1") {
                SystemTrayIcon::instance()->circutor1Action()->setIcon(QIcon(":/Images/bulbDigitalInputEnabled.png"));
            } else if (name() == "Circutor2") {
                SystemTrayIcon::instance()->circutor2Action()->setIcon(QIcon(":/Images/bulbDigitalInputEnabled.png"));
            } else if (name() == "UZ1") {
                SystemTrayIcon::instance()->uz1Action()->setIcon(QIcon(":/Images/bulbDigitalInputEnabled.png"));
            } else if (name() == "MKTN") {
                SystemTrayIcon::instance()->MKTNAction()->setIcon(QIcon(":/Images/bulbDigitalInputEnabled.png"));
            }
        }
    });
}

IConnection *INode::connection() const
{
    return m_connectionPtr.data();
}

void INode::setConnection(IConnection *connection)
{
    if (m_connectionPtr.data() != connection)
        m_connectionPtr = connection;
}

bool INode::hasError() const
{
    return m_lastError != NodeError::noError();
}

NodeError INode::lastError() const
{
    return m_lastError;
}

QString INode::path() const noexcept
{
    return _path;
}

void INode::setPath(const QString &path) noexcept
{
    _path = path;
}

quint8 INode::numberOfRetries() const noexcept
{
    return _numberOfRetries;
}

void INode::setNumberOfRetries(quint8 numberOfRetries) noexcept
{
    _numberOfRetries = numberOfRetries;
}

quint32 INode::timeoutOfRestore() const noexcept
{
    return _timeoutOfRestore;
}

void INode::setTimeoutOfRestore(quint32 timeoutOfRestore) noexcept
{
    _timeoutOfRestore = timeoutOfRestore;
}

quint32 INode::timeout() const noexcept
{
    return _timeout;
}

void INode::setTimeout(quint32 timeout) noexcept
{
    _timeout = timeout;
}
