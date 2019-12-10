#include "Ksu735VariableUI.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

Ksu735VariableUI::Ksu735VariableUI(Ksu735Variable *variable)
{
    setWindowTitle(tr("Параметры переменной"));
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowModality(Qt::WindowModal);

    m_variable = variable;

    //////////////////////////////////////////Создание
    m_nameLabel = new QLabel(tr("Название"), this);
    m_readableLabel = new QLabel(tr("Опращивать"), this);
    m_addressLabel = new QLabel(tr("Адрес"), this);
    //Куда делось?
    //m_divisionPriceLabel = new QLabel(tr("Цена деления шкалы"), this);
    m_maxValueLabel = new QLabel(tr("Макс. значение"), this);
    m_minValueLabel = new QLabel(tr("Мин. значение"), this);
    m_euLabel = new QLabel(tr("Единицы измерения"), this);
    m_writableLabel = new QLabel(tr("Разрешить запись"), this);
    m_descriptionLabel = new QLabel(tr("Описание"), this);

    m_nameLineEdit = new QLineEdit(this);
    m_readableCheck = new QCheckBox(this);
    m_addressSpin = new QSpinBox(this);
    //m_divisionPriceSpin = new QSpinBox(this);
    //m_divisionPriceSpin->setValue(variable->coe)
    //ToDo:
    m_maxValueSpin = new QSpinBox(this);
    m_minValueSpin = new QSpinBox(this);
    m_euLineEdit = new QLineEdit(this);
    m_writableCheck = new QCheckBox(this);
    m_descriptionTextEdit = new QTextEdit(this);
    m_okButton = new QPushButton(tr("Сохранить"), this);
    m_cancelButton = new QPushButton(tr("Отмена"), this);

    //////////////////////////////////////////Инициализация
    m_nameLineEdit->setText(variable->name());
    m_readableCheck->setChecked(variable->isReadable());
    m_addressSpin->setValue(variable->address());
    m_euLineEdit->setText(variable->measureUnit());
    m_writableCheck->setChecked(variable->isWritable());
    m_descriptionTextEdit->setText(variable->description());
    connect(m_okButton, &QPushButton::clicked, this, &Ksu735VariableUI::updateProperties);
    connect(m_cancelButton, &QPushButton::clicked, this, &QWidget::close);

    //////////////////////////////////////////Размещение
    QFormLayout *editLayout = new QFormLayout();
    editLayout->addRow(m_nameLabel, m_nameLineEdit);
    editLayout->addRow(m_readableLabel, m_readableCheck);
    editLayout->addRow(m_addressLabel, m_addressSpin);
    editLayout->addRow(m_minValueLabel, m_minValueSpin);
    editLayout->addRow(m_maxValueLabel, m_maxValueSpin);
    editLayout->addRow(m_euLabel, m_euLineEdit);
    editLayout->addRow(m_writableLabel, m_writableCheck);
    editLayout->addRow(m_descriptionLabel, m_descriptionTextEdit);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_okButton);
    buttonsLayout->addWidget(m_cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(editLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);
}

Ksu735VariableUI::~Ksu735VariableUI()
{

}

void Ksu735VariableUI::updateProperties()
{
    m_variable->setName(m_nameLineEdit->text());
    m_variable->setReadable(m_readableCheck->isChecked());
    m_variable->setAddress(m_addressSpin->value());
    //min
    //max
    m_variable->setMeasureUnit(m_euLineEdit->text());
    m_variable->setWritable(m_writableCheck->isChecked());
    m_variable->setDescription(m_descriptionTextEdit->toPlainText());

    m_variable->getConfigNode()->setAttribute("name", m_variable->name());
    m_variable->getConfigNode()->setAttribute("readavle", m_variable->isReadable());
    m_variable->getConfigNode()->setAttribute("address", m_variable->address());
    //min
    //max
    m_variable->getConfigNode()->setAttribute("measureUnit", m_variable->measureUnit());
    m_variable->getConfigNode()->setAttribute("writable", m_variable->isWritable());
    m_variable->getConfigNode()->setAttribute("description", m_variable->description());
}
