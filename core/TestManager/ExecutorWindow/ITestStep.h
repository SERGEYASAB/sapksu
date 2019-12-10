#ifndef ITESTSTEP_H
#define ITESTSTEP_H

#include "Common/PrefixHeader.h"

class ITestStep : public QObject
{
    Q_OBJECT

public:
    explicit ITestStep(QObject *parent = Q_NULLPTR);
    virtual ~ITestStep();

    enum StepStatus {
        None,
        Executing,  // выполняется
        Success,    // выполнен успешно
        Failure,    // выполнен с ошибкой
        Exception   // выполнен с исключением
    };
    Q_ENUM(StepStatus)

    QString name() const;
    void setName(const QString &name);

    QString textProgram() const;
    void setTextProgram(const QString &textProgram);

    QString message() const;
    void setMessage(const QString &message);

    StepStatus status() const;
    void setStatus(const StepStatus &status);

private:
    QString m_name;         // Название шага
    QString m_textProgram;  // Содержимое программы
    QString m_message;      // Состояние ошибки (можно использовать как возвращаемое значение)
    StepStatus m_status;    // Состояние шага

public:
    virtual void run() = 0;

Q_SIGNALS:
    void messageChanged(const QString &message);
    void statusChanged(const ITestStep::StepStatus &status);
};

#endif // ITESTSTEP_H
