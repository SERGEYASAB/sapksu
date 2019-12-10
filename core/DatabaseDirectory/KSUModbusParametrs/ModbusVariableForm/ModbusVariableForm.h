#ifndef MODBUSVARIABLEFORM_H
#define MODBUSVARIABLEFORM_H

#include "Common/PrefixHeader.h"

class ParameterAliasWidget;

class ModbusVariableForm : public QWidget
{
    Q_OBJECT

public:
    ModbusVariableForm(QDomNode node, QWidget *parent = nullptr);

signals:
    void elementAdded(QDomElement &);
    void elementSaved(QDomElement &);
    void closed();

public slots:
    void setVariableStrategy(int type);

    void save();

protected:
    QVBoxLayout *mainLayout;

    //common
    QGroupBox *commonGroupBox;
    QFormLayout *commonLayout;

    QLineEdit *nameLineEdit;
    QTextEdit *descriptionTextEdit;
    ParameterAliasWidget *aliasLineEdit;
    QLineEdit *measureUnitLineEdit;

    QHBoxLayout *accessLayout;
    QCheckBox *readAccessCheckBox;
    QCheckBox *writeAccessCheckBox;

    QCheckBox *enabledCheckBox;

    QHBoxLayout *periodLayout;
    QCheckBox *periodEnableCheckBox;
    QSpinBox *periodSpinBox;
    QLabel *periodUnitLabel;

    //features
    QGroupBox *featuresGroubBox;
    QFormLayout *featuresLayout;

    QHBoxLayout *hexAddressLayout;
    QSpinBox *hexAddressSpinBox;
    QLabel *hexAddressLabel;

    QHBoxLayout *decAddressLayout;
    QSpinBox *decAddressSpinBox;
    QLabel *decAddressLabel;

    QComboBox *byteOrderComboBox;

    QGridLayout *typeLayout;
    QButtonGroup *variableTypeGroup;
    QRadioButton *standartTypeRadioButton;
    QRadioButton *linearTypeRadioButton;
    QRadioButton *registerTypeRadioButton;
    QRadioButton *enumTypeRadioButton;
    QRadioButton *bitsetTypeRadioButton;

    //parametrs
    QGroupBox *parametrsGroupBox;
    QFormLayout *parametrsLayout;

    QFormLayout *dataTypeLayout;
    QComboBox *deviceDataTypeComboBox;
    QComboBox *serverDataTypeComboBox;

    QLabel *minimumLabel;
    QLineEdit *minimumLineEdit;
    QLabel *maximumLabel;
    QLineEdit *maximumLineEdit;
    QLabel *defaultValueLabel;
    QLineEdit *defaultValueLineEdit;
    QLabel *permissibleVariationLabel;
    QLineEdit *permissibleVariationLineEdit;

    QLabel *stringRegistersCountLabel;
    QSpinBox *stringRegistersCountSpinBox;
    QLabel *floatPrecisionLabel;
    QSpinBox *floatPrecisionSpinBox;

    //ext parametrs (depends of type)
    QWidget *extendedParametrsWidget;
    QVBoxLayout *extendedParametrsLayout;

    QWidget *linearParametrsWidget;
    QFormLayout *linearParametrsLayout;
    QDoubleSpinBox *coefficientKSpinBox;
    QDoubleSpinBox *coefficientBSpinBox;
    QPushButton *changeLinearCoefficientsButton;

    QWidget *registerParametrsWidget;
    QHBoxLayout *significantBitsLayout;
    QLabel *significantBitsLabel;
    QLabel *startBitLabel;
    QSpinBox *startBitSpinBox;
    QLabel *endBitLabel;
    QSpinBox *endBitSpinBox;
    QLabel *significantBitsNotationLabel;

    QWidget *enumeratedParametrsWidget;
    QGridLayout *enumeratedParametrsLayout;
    QTableView *extendedParametrsTableView;
    QPushButton *appendButton;
    QPushButton *removeButton;

    QDialogButtonBox *applyButtonBox;

private slots:
    void appendClicked();
    void removeClicked();

    void deviceDataTypeComboBoxCurrentIndexChanged(int index);
    void serverDataTypeComboBoxCurrentIndexChanged(int index);

private:
    void setupUi();
    void setupCommonGroupBox();
    void setupFeaturesGroubBox();
    void setupParametrsGroupBox();
    void setupButtonBox();
    void setupLinearParametrsWidget();
    void setupRegisterParametrsWidget();
    void setupEnumeratedParametrsWidget();

    void fillDataTypes();

    void traverseXMLElement(const QDomElement &element);
    void parseAndFillMinMaxDefaultValues();
    void parseEnumStrategyValues();
    void parseBitsetStrategyValues();

    void saveEnumValues();
    void saveBitsetValues();

    QString stringHexCode(quint64 value) const;

private:
    QDomElement m_variableElement;
    QDomNode m_parentNode;

    QStandardItemModel *m_enumStrategyValuesModel;
    QStandardItemModel *m_bitsetStrategyValuesModel;

    int m_strategyType = 0;
    QSet<quint64> m_enumFlags;
    QSet<quint64> m_bitsetFlags;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MODBUSVARIBLEFORM_H
