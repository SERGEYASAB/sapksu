#ifndef ANALOGOUTPUTWIDGET_H
#define ANALOGOUTPUTWIDGET_H

#include "Common/PrefixHeader.h"

class IVariable;

class AnalogOutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AnalogOutputWidget(uint outputNumber,
                                const QString &enableVoltageModeAlias,
                                const QString &enableAmperageModeAlias,
                                const QString &voltageValueAlias,
                                const QString &amperageValueAlias,
                                QWidget *parent = 0);

    ~AnalogOutputWidget();

private slots:
    void disableOutput(bool checked);
    void enable0_10V(bool checked);
    void enable4_20mA(bool checked);
    void voltageOutputValueChanged(double value);
    void amperageOutputValueChanged(double value);
    void voltageCorrectionFactorValueChanged(double value);
    void amperageCorrectionFactorValueChanged(double value);

private:
    void setVoltage();
    void setAmperage();

private: // UI
    QGroupBox *m_groupBox = nullptr;
    QRadioButton *m_disableOutputRadioButton = nullptr;

    QRadioButton *m_enable0_10VRadioButton = nullptr;
    QDoubleSpinBox *m_voltageSpinBox = nullptr;
    QDoubleSpinBox *m_voltageCorrectionFactorSpinBox = nullptr;

    QRadioButton *m_enable4_20mARadioButton = nullptr;
    QDoubleSpinBox *m_amperageSpinBox = nullptr;
    QDoubleSpinBox *m_amperageCorrectionFactorSpinBox = nullptr;

private: // logic
    uint m_outputNumber = 0;
    IVariable *m_enableVoltageModeVariable = nullptr;
    IVariable *m_enableAmperageModeVariable = nullptr;
    IVariable *m_voltageOutputValueVariable = nullptr;
    IVariable *m_amperageOutputValueVariable = nullptr;
    bool init = true;
};

#endif // ANALOGOUTPUTWIDGET_H
