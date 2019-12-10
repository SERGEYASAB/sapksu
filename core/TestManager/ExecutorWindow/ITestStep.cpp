#include "ITestStep.h"

ITestStep::ITestStep(QObject *parent)
{
    setStatus(StepStatus::None);

}

ITestStep::~ITestStep()
{

}

QString ITestStep::name() const
{
    return m_name;
}

void ITestStep::setName(const QString &name)
{
    m_name = name;
}

QString ITestStep::message() const
{
    return m_message;
}

void ITestStep::setMessage(const QString &error)
{
    m_message = error;
    emit messageChanged(error);
}

ITestStep::StepStatus ITestStep::status() const
{
    return m_status;
}

void ITestStep::setStatus(const StepStatus &status)
{
    if (m_status == status)
        return;

    m_status = status;

    emit statusChanged(status);
}

QString ITestStep::textProgram() const
{
    return m_textProgram;
}

void ITestStep::setTextProgram(const QString &textProgram)
{
    m_textProgram = textProgram;
}
