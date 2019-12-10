#ifndef KSU735VARIABLEUI_H
#define KSU735VARIABLEUI_H

#include <QtWidgets>

class Ksu735Variable;

class Ksu735VariableUI : public QWidget
{
    Q_OBJECT

public:
    Ksu735VariableUI(Ksu735Variable *variable);
    virtual ~Ksu735VariableUI();

private:
    Ksu735Variable *m_variable;

    QLabel *m_nameLabel;
    QLabel *m_readableLabel;
    QLabel *m_addressLabel;
    QLabel *m_divisionPriceLabel;
    QLabel *m_maxValueLabel;
    QLabel *m_minValueLabel;
    QLabel *m_euLabel;
    QLabel *m_writableLabel;
    QLabel *m_descriptionLabel;

    QLineEdit *m_nameLineEdit;
    QCheckBox *m_readableCheck;
    QSpinBox *m_addressSpin;
    QSpinBox *m_divisionPriceSpin;
    QSpinBox *m_maxValueSpin;
    QSpinBox *m_minValueSpin;
    QLineEdit *m_euLineEdit;
    QCheckBox *m_writableCheck;
    QTextEdit *m_descriptionTextEdit;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

private slots:
    void updateProperties();
};

#endif // KSU735VARIABLEUI_H
