#ifndef MODBUSVARIABLEUI_H
#define MODBUSVARIABLEUI_H

#include <QtWidgets>

class ModbusVariable;

class ModbusVariableUI : public QWidget
{
    Q_OBJECT
public:
    explicit ModbusVariableUI(ModbusVariable *variable);
    virtual ~ModbusVariableUI();

private:
    ModbusVariable *m_variable;

    QLabel *m_nameLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_addressLabel;
    QLabel *m_dataTypeLabel;
    QLabel *m_byteOrderLabel;
    QLabel *m_readableLabel;
    QLabel *m_writableLabel;
    QLabel *m_maxValueLabel;
    QLabel *m_minValueLabel;
    QLabel *m_euLabel;
    QLabel *m_signBitsLabel;

    QLineEdit *m_nameLineEdit;
    QTextEdit *m_descriptionTextEdit;
    QSpinBox *m_addressSpin;
    QComboBox *m_dataTypeCombo;
    QComboBox *m_byteOrderCombo;
    QCheckBox *m_readableCheck;
    QCheckBox *m_writableCheck;
    QSpinBox *m_maxValueSpin;
    QSpinBox *m_minValueSpin;
    QLineEdit *m_euLineEdit;
    QSpinBox *m_firstBitSpin;
    QSpinBox *m_lastBitSpin;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

private slots:
    void updateProperties();
};

#endif // MODBUSVARIABLEUI_H
