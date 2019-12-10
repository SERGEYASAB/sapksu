#ifndef MODBUSVARIABLEEDITBITSETVALUE_H
#define MODBUSVARIABLEEDITBITSETVALUE_H

#include "Common/PrefixHeader.h"

class ModbusVariableEditBitsetValue : public QWidget
{
    Q_OBJECT

public:
    ModbusVariableEditBitsetValue(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    void setCodeLabelText(const QString &text);
    QString codeLabelText() const { return m_codeLabel->text(); }

    void setCode(const QString &code) { m_codeLineEdit->setText(code); }
    QString code() const { return m_codeLineEdit->text(); }

    void setCodeEditable(bool editable) { m_codeLineEdit->setEnabled(editable); }
    bool codeIsEditable() { return m_codeLineEdit->isEnabled(); }

    void setValueLabelText(const QString &text);
    QString valueLabelText() const { return m_valueLabel->text(); }

    void setValue(const QString &value) { m_valueLineEdit->setText(value); }
    QString value() const { return m_valueLineEdit->text(); }

signals:
    void accepted();

private slots:
    void okClicked();

private:
    void adjustSizeToMinimum();

private:
    QLabel *m_codeLabel;
    QLineEdit *m_codeLineEdit;

    QLabel *m_valueLabel;
    QLineEdit *m_valueLineEdit;

    QPushButton *okButton;
    QPushButton *cancelButton;
    QDialogButtonBox *m_buttonBox;
};


#endif // MODBUSVARIABLEEDITBITSETVALUE_H
