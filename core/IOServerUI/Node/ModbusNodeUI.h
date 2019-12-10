#ifndef MODBUSNODEUI_H
#define MODBUSNODEUI_H

#include <QtWidgets>

class ModbusNode;

class ModbusNodeUI : public QWidget
{
    Q_OBJECT

public:
     explicit ModbusNodeUI(ModbusNode *node);
     virtual ~ModbusNodeUI();

private:
    ModbusNode *m_node;

    QLabel *m_nameLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_readFunctionLabel;
    QLabel *m_writeFunctionLabel;
    QLabel *m_addressLabel;
    QLabel *m_periodLabel;
    QLabel *m_groupRequestLabel;
    QLabel *m_maxLenghtPduLabel;
    QLabel *m_relayAddressLabel;
    QLabel *m_firstCommingAddressLabel;
    QLabel *m_timeoutOfRestoreLabel;
    QLabel *m_readableLabel;
    QLabel *m_numberOfRetriesLabel;
    QLabel *m_timeoutLabel;
    QLabel *m_hollowRegistersCountLabel;

    QLineEdit *m_nameLineEdit;
    QTextEdit *m_descriptionTextEdit;
    QComboBox *m_readFunctionCombo;
    QComboBox *m_writeFunctionCombo;
    QSpinBox *m_addressSpin;
    QSpinBox *m_periodSpin;
    QCheckBox *m_groupRequestCheck;
    QSpinBox *m_maxLengthPduSpin;
    QLineEdit *m_relayAddressLineEdit;
    QLineEdit *m_firstCommingAddressLineEdit;
    QSpinBox *m_timeoutOfRestoreSpin;
    QCheckBox *m_readableCheck;
    QSpinBox *m_numberOfRetriesSpin;
    QSpinBox *m_timeoutSpin;
    QCheckBox *m_hollowRegistersCountCheck;

    QPushButton *m_okButton;
private slots:
    void updateProperties();
};

#endif // MODBUSNODEUI_H
