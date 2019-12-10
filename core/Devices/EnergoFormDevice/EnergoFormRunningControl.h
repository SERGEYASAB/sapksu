#ifndef ENERGOFORMRUNNINGCONTROL_H
#define ENERGOFORMRUNNINGCONTROL_H

#include "Common/PrefixHeader.h"

class EnergoFormRunningControl : public QObject
{
    Q_OBJECT

public:
    static EnergoFormRunningControl *instance();

    void runEnergoForm();

private:
    explicit EnergoFormRunningControl(QObject *parent = 0);
    ~EnergoFormRunningControl();

    QProcess *process = nullptr;

    EnergoFormRunningControl(EnergoFormRunningControl const&) = delete;             // Copy construct
    EnergoFormRunningControl(EnergoFormRunningControl&&) = delete;                  // Move construct
    EnergoFormRunningControl& operator=(EnergoFormRunningControl const&) = delete;  // Copy assign
    EnergoFormRunningControl& operator=(EnergoFormRunningControl &&) = delete;      // Move assign
};


#endif // ENERGOFORMRUNNINGCONTROL_H
