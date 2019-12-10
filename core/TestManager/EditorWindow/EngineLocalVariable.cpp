#include "EngineLocalVariable.h"

EngineLocalVariable::EngineLocalVariable(QObject *parent) : QObject(parent),
  m_value(0),
  m_writable(false)
{

}

EngineLocalVariable::~EngineLocalVariable()
{

}

QVariant EngineLocalVariable::value() const
{
    return m_value;
}

void EngineLocalVariable::writeValue(const QVariant &value)
{
    m_value = value;
    qDebug()<<"CHANGED";
    emit valueChanged(this);
}

QString EngineLocalVariable::name() const
{
    return m_name;
}

void EngineLocalVariable::setName(const QString &name)
{
    m_name = name;
}

bool EngineLocalVariable::isWritable() const
{
    return m_writable;
}

void EngineLocalVariable::setWritable(bool writable)
{
    m_writable = writable;
}

QString EngineLocalVariable::description() const
{
    return m_description;
}

void EngineLocalVariable::setDescription(const QString &description)
{
    m_description = description;
}
