#include "ModbusVariableEditBitsetValue.h"

ModbusVariableEditBitsetValue::ModbusVariableEditBitsetValue(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f)
{
    m_codeLabel = new QLabel(this);
    m_codeLabel->setObjectName(QStringLiteral("m_codeLabel"));

    m_codeLineEdit = new QLineEdit(this);
    m_codeLineEdit->setObjectName(QStringLiteral("m_codeLineEdit"));
    m_codeLineEdit->setValidator(new QIntValidator(0, std::numeric_limits<int>::max(), m_codeLineEdit));

    m_valueLabel = new QLabel(this);
    m_valueLabel->setObjectName(QStringLiteral("m_valueLabel"));

    m_valueLineEdit = new QLineEdit(this);
    m_valueLineEdit->setObjectName(QStringLiteral("m_valueLineEdit"));

    okButton = new QPushButton(tr("Ок"));
    connect(okButton, &QPushButton::clicked, this, &ModbusVariableEditBitsetValue::okClicked);

    cancelButton = new QPushButton(tr("Отмена"));
    connect(cancelButton, &QPushButton::clicked, this, &ModbusVariableEditBitsetValue::close);

    m_buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    m_buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
    m_buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_codeLabel);
    layout->addWidget(m_codeLineEdit);
    layout->addSpacing(1);
    layout->addWidget(m_valueLabel);
    layout->addWidget(m_valueLineEdit);
    layout->addWidget(m_buttonBox);

    setLayout(layout);
}

void ModbusVariableEditBitsetValue::setCodeLabelText(const QString &text) {
    m_codeLabel->setText(text);
    adjustSizeToMinimum();
}

void ModbusVariableEditBitsetValue::setValueLabelText(const QString &text) {
    m_valueLabel->setText(text);
    adjustSizeToMinimum();
}

void ModbusVariableEditBitsetValue::okClicked()
{
    emit accepted();
}

void ModbusVariableEditBitsetValue::adjustSizeToMinimum()
{
    adjustSize();
    setFixedHeight(height());
}
