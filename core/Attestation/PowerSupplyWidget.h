#ifndef POWERSUPPLYWIDGET_H
#define POWERSUPPLYWIDGET_H

#include "Common/PrefixHeader.h"

class IVariable;

class PowerSupplyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PowerSupplyWidget(QWidget *parent = 0);
    ~PowerSupplyWidget();

private slots: ///--- logic slots ---///
    void pvKsu220ACVoltageValueChanged();
    void pvKsu15ACVoltageValueChanged();
    void pvKsu24DCVoltageValueChanged();
    void pvKsuCircuit20ACVoltageValueChanged();
    void pv220ACVoltageValueChanged();

private slots: ///--- UI slots ----///
    void ksuDisableOutput(bool checked);
    void ksuEnable220AC(bool checked);
    void ksuEnable15AC(bool checked);
    void ksuEnable24DC(bool checked);


private slots: ///--- UI slots ---///
    void ksuDisableCircuit1Output(bool checked);
    void ksuEnableCircuit1Output(bool checked);

    void ksuDisableCircuit2Output(bool checked);
    void ksuEnableCircuit2Output(bool checked);

    void tmsDisableOutput(bool checked);
    void tmsEnableOutput(bool checked);

private slots: ///--- spin box value changed slots ---///
    void ksuVoltage220ACPredeterminedSpinBoxValueChanged(int value);
    void ksuVoltage15ACPredeterminedSpinBoxValueChanged(int value);
    void ksuVoltage24DCPredeterminedSpinBoxValueChanged(int value);
    void ksuCircuit1PredeterminedSpinBoxValueChanged(int value);


    void ksuVoltage220ACCorrectionFactorSpinBoxValueChanged(double value);
    void ksuVoltage15ACCorrectionFactorSpinBoxValueChanged(double value);
    void ksuVoltage24DCCorrectionFactorSpinBoxValueChanged(double value);
    void ksuCircuit1CorrectionFactorSpinBoxValueChanged(double value);

private:
    void setKsuVoltage220AC();
    void setKsuVoltage15AC();
    void setKsuVoltage24DC();
    void setKsuCircuit1Voltage20AC();

private:
    void setupKsuPowerSupplyOutputGroupBoxUI();
    void setupKsuPowerSupplyCircuit1GroupBoxUI();
    void setupKsuPowerSupplyCircuit2GroupBoxUI();
    void setupTmsPowerSupplyGroupBoxUI();

    void setupVariables();

    bool showEnablePowerQuestion(const QString &outputName);

private: ///--- logic ---///
    IVariable *m_pvKsu220ACVoltageVariable = nullptr;
    IVariable *m_pvKsu15ACVoltageVariable = nullptr;
    IVariable *m_pvKsu24DCVoltageVariable = nullptr;
    IVariable *m_pvKsuCircuit20ACVoltageVariable = nullptr;
    IVariable *m_pv220ACVoltageVariable = nullptr;

    IVariable *m_uz1WorkPermitVariable = nullptr;
    IVariable *m_uz2WorkPermitVariable = nullptr;

    IVariable *m_enableKsu220ACOutputVariable = nullptr;
    IVariable *m_enableKsu15ACOutputVariable = nullptr;
    IVariable *m_enableKsu24DCOutputVariable = nullptr;

    IVariable *m_enableKsu220ACAdditionalOutputVariable = nullptr;
    IVariable *m_enableKsu24DCAdditionalOutputVariable = nullptr;

    IVariable *m_enableKsuCircuit1OutputVariable = nullptr;
    IVariable *m_enableKsuCircuit2OutputVariable = nullptr;
    IVariable *m_enableTmsOutputVariable = nullptr;

    IVariable *m_uz1WriteUVariable = nullptr;
    IVariable *m_uz1WriteFreqVariable = nullptr;
    IVariable *m_uz1StartStopVariable = nullptr;

    IVariable *m_uz2WriteUVariable = nullptr;
    IVariable *m_uz2WriteFreqVariable = nullptr;
    IVariable *m_uz2StartStopVariable = nullptr;

private: ///--- выход 1 питания ксу ---///
    QGroupBox *m_ksuPowerSupplyOutputGroupBox = nullptr;

    QRadioButton *m_ksuDisableOutputRadioButton = nullptr;

    QRadioButton *m_ksuEnable220ACRadioButton = nullptr;
    QSpinBox *m_ksuVoltage220ACPredeterminedSpinBox = nullptr;
    QLineEdit *m_ksuVoltage220ACMeasuredLineEdit = nullptr;
    QDoubleSpinBox *m_ksuVoltage220ACCorrectionFactorSpinBox = nullptr;

    QRadioButton *m_ksuEnable15ACRadioButton = nullptr;
    QSpinBox *m_ksuVoltage15ACPredeterminedSpinBox = nullptr;
    QLineEdit *m_ksuVoltage15ACMeasuredLineEdit = nullptr;
    QDoubleSpinBox *m_ksuVoltage15ACCorrectionFactorSpinBox = nullptr;

    QRadioButton *m_ksuEnable24DCRadioButton = nullptr;
    QSpinBox *m_ksuVoltage24DCPredeterminedSpinBox = nullptr;
    QLineEdit *m_ksuVoltage24DCMeasuredLineEdit = nullptr;
    QDoubleSpinBox *m_ksuVoltage24DCCorrectionFactorSpinBox = nullptr;

private: ///--- питаниe цепей управления ксу 1 ---///
    QGroupBox *m_ksuPowerSupplyCircuit1GroupBox = nullptr;

    QRadioButton *m_ksuDisableCircuit1RadioButton = nullptr;

    QRadioButton *m_ksuEnableCircuit1RadioButton = nullptr;
    QSpinBox *m_ksuCircuit1PredeterminedSpinBox = nullptr;
    QLineEdit *m_ksuCircuit1MeasuredLineEdit = nullptr;
    QDoubleSpinBox *m_ksuCircuit1CorrectionFactorSpinBox = nullptr;

private: ///--- питаниe цепей управления ксу 2 ---///
    QGroupBox *m_ksuPowerSupplyCircuit2GroupBox = nullptr;
    QRadioButton *m_ksuDisableCircuit2RadioButton = nullptr;
    QRadioButton *m_ksuEnableCircuit2RadioButton = nullptr;
    QLineEdit *m_ksuCircuit2MeasuredLineEdit = nullptr;

private: ///--- питаниe тмс ---///
    QGroupBox *m_tmsPowerSupplyGroupBox = nullptr;
    QRadioButton *m_tmsDisableRadioButton = nullptr;
    QRadioButton *m_tmsEnableRadioButton = nullptr;
    QLineEdit *m_tmsMeasuredLineEdit = nullptr;
};

#endif // POWERSUPPLYWIDGET_H
