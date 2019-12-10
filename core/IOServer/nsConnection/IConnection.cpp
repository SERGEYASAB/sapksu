#include "IConnection.h"

#include <QMetaEnum>

IConnection::IConnection(QObject *parent) : QObject(parent)
{

}

IConnection::IConnection(QDomElement &domElement, QObject *parent) : QObject(parent)
{
    m_domElement = domElement;
}

IConnection::~IConnection()
{
}

QString IConnection::name() const
{
    return m_name;
}

void IConnection::setName(const QString &name)
{
    m_name = name;
}

QString IConnection::description() const
{
    return m_description;
}

void IConnection::setDescription(const QString &description)
{
    m_description = description;
}

QDomElement IConnection::domElement()
{
    return m_domElement;
}

quint32 IConnection::nextRequestDelay() const noexcept
{
    return m_nextRequestDelay;
}

void IConnection::setNextRequestDelay(quint32 delay) noexcept
{
    m_nextRequestDelay = delay;
}

QString IConnection::id() const
{
    return m_id;
}

void IConnection::setId(const QString &id)
{
    m_id = id;
}
