#ifndef ATTESTATIONWINDOW_H
#define ATTESTATIONWINDOW_H

#include "Common/PrefixHeader.h"

class IVariable;

class AttestationWindow : public QTabWidget
{
    Q_OBJECT

public:
    explicit AttestationWindow(QWidget *parent = nullptr);

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void currentIndexChanged(int index);

private:
    IVariable *m_enableDigitalOutputs1Variable = nullptr;
    IVariable *m_enableDigitalOutputs2Variable = nullptr;
    IVariable *m_enableDigitalInputsVariable = nullptr;
};

#endif // ATTESTATIONWINDOW_H
