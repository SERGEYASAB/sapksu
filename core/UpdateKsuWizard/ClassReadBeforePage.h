#ifndef CLASSREADBEFOREPAGE_H
#define CLASSREADBEFOREPAGE_H

#include "Common/PrefixHeader.h"

class ModbusKsuManualFullTestingWidget;
class Ksu735ManualFullTestingWidget;
struct ModbusKsuManualTestingParametr;
struct Ksu735ManualFullTestingParametr;
class IVariable;

class ClassReadBeforePage: public QWizardPage
{
    Q_OBJECT

public:
    explicit ClassReadBeforePage(QWidget *parent = nullptr);
    bool isComplete() const override;

    QString getPedState() const;

public slots:
    void cancelTesting();

protected:
    void initializePage() override;

private:
    QString pedState = "";
    ModbusKsuManualFullTestingWidget *m_tempMbTestingWidget = nullptr;
    Ksu735ManualFullTestingWidget *m_temp735TestingWidget = nullptr;
    bool m_completed = false;
    void setCompleted(bool state);
    void init();

signals:
    void started();
    void finished(QList<IVariable *> vars);
};

#endif // CLASSREADBEFOREPAGE_H
