#include "EnergoFormRunningControl.h"
#include "ReportManager/OperatorClickReport.h"

EnergoFormRunningControl *EnergoFormRunningControl::instance()
{
    static EnergoFormRunningControl singletone;
    return &singletone;
}

EnergoFormRunningControl::~EnergoFormRunningControl()
{
    process->kill();
    process->deleteLater();
}

void EnergoFormRunningControl::runEnergoForm()
{
    if (process == nullptr)
        process = new QProcess();

    QString file = ENERGOFORM_PATH;
    process->start(file);
    OperatorClickReport::instance()->write(QStringLiteral("Запуск окна энергоформы"));
}

EnergoFormRunningControl::EnergoFormRunningControl(QObject *parent)
{

}
