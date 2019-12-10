#include "ScriptDelayMessageBox.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"

static const int TIME_MSUNIT = 500;

ScriptDelayMessageBox::ScriptDelayMessageBox(quint32 delay, const QString &message, QWidget *parent)
    : QMessageBox(parent),
      m_delayms(delay),
      m_message(message)
{
    setWindowTitle("Функция таймер обратного отчета");
    setWindowIcon(QIcon(":/Images/irz_logo.png"));
    setStandardButtons(StandardButton::Ok);
    setIcon(QMessageBox::Information);
    setText(message);

    connect(&timer, &QTimer::timeout, this, &ScriptDelayMessageBox::checkTimer);
}

ScriptDelayMessageBox::ScriptDelayMessageBox(const QString &alias, const QVariant &value, const QString &message, QWidget *parent)
    : QMessageBox(parent),
      m_aliasName(alias),
      m_value(value),
      m_message(message)
{
    m_delayms = 0;

    setWindowTitle("Функция ожидания события");
    setWindowIcon(QIcon(":/Images/irz_logo.png"));
    setStandardButtons(StandardButton::Abort);
    setIcon(QMessageBox::Information);
    setText(message);

    m_variable = IOServer::instance()->variableByAlias(m_aliasName);
    connect(&timer, &QTimer::timeout, this, &ScriptDelayMessageBox::waitCondion);
}

ScriptDelayMessageBox::ScriptDelayMessageBox(const QString &alias, const QVariant &value, const QString &message, quint32 delay, QWidget *parent)
    : QMessageBox(parent),
      m_aliasName(alias),
      m_value(value),
      m_message(message),
      m_delayms(delay)
{
    setWindowTitle("Функция ожидания события");
    setWindowIcon(QIcon(":/Images/irz_logo.png"));
    setStandardButtons(StandardButton::Abort);
    setIcon(QMessageBox::Information);
    setText(message);

    m_variable = IOServer::instance()->variableByAlias(m_aliasName);
    connect(&timer, &QTimer::timeout, this, &ScriptDelayMessageBox::waitCondionDelay);
}

void ScriptDelayMessageBox::checkTimer()
{
    m_delayms -= timer.interval();

    if (m_delayms > 0) {
        setText(m_message);
        setInformativeText(QString("%1 ms").arg(m_delayms));
        timer.start();
    } else {
        done(1);
    }
}

void ScriptDelayMessageBox::waitCondion()
{   
    if (m_variable == nullptr) {
        done(0);
    } else {
        if (m_variable->readValue() != m_value) {
            timer.start();
        } else {
            done(1);
        }
    }

}

void ScriptDelayMessageBox::waitCondionDelay()
{
    if (m_variable == nullptr || m_delayms < 0)
        done(0);

    QTime readTimer;
    readTimer.start();
    m_delayms -= timer.interval(); //Т.к. первый запуск тоже по таймеру
    if (m_delayms < 0)
        return;
    setInformativeText(QString("%1 ms").arg(m_delayms));
    if (m_variable->readValue() == m_value)
        done(1);
    m_delayms -= readTimer.elapsed();
    timer.start();
}

void ScriptDelayMessageBox::showEvent(QShowEvent *event)
{
    setText(m_message);

    if (m_delayms != 0) {
        setInformativeText(QString("%1 ms").arg(m_delayms));
    }

    timer.setTimerType(Qt::PreciseTimer);
    timer.setInterval(TIME_MSUNIT);
    timer.start();

    QMessageBox::showEvent(event);
}
