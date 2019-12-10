#ifndef ANALOGINPUTWIDGET_H
#define ANALOGINPUTWIDGET_H

#include "Common/PrefixHeader.h"

class IVariable;

class AnalogInputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AnalogInputWidget(uint inputNumber,
                               const QString &disableInputAlias,
                               const QString &recalculationAlias,
                               const QString &inputValueAlias,
                               QWidget *parent = 0);
    ~AnalogInputWidget();


private slots:
    void disableInput(bool checked);
    void enable0_10V(bool checked);
    void enable4_20mA(bool checked);
    void inputValueChanged();
    void voltageCorrectionFactorValueChanged(double value);
    void amperageCorrectionFactorValueChanged(double value);

private: // UI
    QGroupBox *m_groupBox = nullptr;

    QRadioButton *disableOutputRadioButton = nullptr;

    QRadioButton *m_enable0_10VRadioButton = nullptr;
    QLineEdit *m_voltageLineEdit = nullptr;
    QDoubleSpinBox *m_voltageCorrectionFactorSpinBox = nullptr;

    QRadioButton *m_enable4_20mARadioButton = nullptr;
    QLineEdit *m_amperageLineEdit = nullptr;
    QDoubleSpinBox *m_amperageCorrectionFactorSpinBox = nullptr;

private: // logic
    uint m_inputNumber = 0;
    IVariable *m_disableInputVariable = nullptr;
    IVariable *m_recalculationVariable = nullptr;
    IVariable *m_inputValueVariable = nullptr;
    bool init = true;
};

#endif // ANALOGINPUTWIDGET_H
