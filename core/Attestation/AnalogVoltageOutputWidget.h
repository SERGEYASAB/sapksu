#ifndef ANALOGVOLTAGEOUTPUTWIDGET_H
#define ANALOGVOLTAGEOUTPUTWIDGET_H

#include "Common/PrefixHeader.h"

class IVariable;

class AnalogVoltageOutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AnalogVoltageOutputWidget(uint outputNumber,
                                       const QString &voltageValueAlias,
                                       QWidget *parent = 0);

    ~AnalogVoltageOutputWidget();

private slots:
    void voltageOutputValueChanged(double value);
    void voltageCorrectionFactorValueChanged(double value);

private:
    void setVoltage();

private: // UI
    QGroupBox *m_groupBox = nullptr;

    QRadioButton *m_enable0_10VRadioButton = nullptr;
    QDoubleSpinBox *m_voltageSpinBox = nullptr;
    QDoubleSpinBox *m_voltageCorrectionFactorSpinBox = nullptr;

private: // logic
    uint m_outputNumber = 0;
    IVariable *m_enableVoltageModeVariable = nullptr;
    IVariable *m_voltageOutputValueVariable = nullptr;
};

#endif // ANALOGVOLTAGEOUTPUTWIDGET_H
