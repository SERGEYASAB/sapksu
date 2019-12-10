#ifndef SCRIPTDELAYMESSAGEBOX_H
#define SCRIPTDELAYMESSAGEBOX_H

#include "Common/PrefixHeader.h"

class IVariable;

class ScriptDelayMessageBox : public QMessageBox
{
    Q_OBJECT

public:
    explicit ScriptDelayMessageBox(quint32 delay, const QString &message, QWidget *parent = Q_NULLPTR);
    explicit ScriptDelayMessageBox(const QString &alias, const QVariant &value, const QString &message, QWidget *parent = Q_NULLPTR);
    explicit ScriptDelayMessageBox(const QString &alias, const QVariant &value, const QString &message, quint32 delay, QWidget *parent = Q_NULLPTR);

private:
    //timer message
    int m_delayms;
    QString m_message;

    //condition message
    QString m_aliasName;
    QVariant m_value;
    IVariable *m_variable = nullptr;
    QTimer timer;

private slots:
    void checkTimer();
    void waitCondion();
    void waitCondionDelay();

protected:
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
};

#endif // SCRIPTDELAYMESSAGEBOX_H
