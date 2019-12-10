#include "ModbusVariableUI.h"
#include "IOServer/nsVariable/ModbusVariable.h"

ModbusVariableUI::ModbusVariableUI(ModbusVariable *variable)
{
    setWindowTitle(tr("Параметры переменной"));
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowModality(Qt::WindowModal);

    m_variable = variable;

    //////////////////////////////////////////Создание
    m_nameLabel = new QLabel(tr("Название"), this);
    m_descriptionLabel = new QLabel(tr("Описание"), this);
    m_addressLabel = new QLabel(tr("Адрес"), this);
    m_dataTypeLabel = new QLabel(tr("Тип"), this);
    m_byteOrderLabel = new QLabel(tr("Порядок байт"), this);
    m_signBitsLabel = new QLabel(tr("Значимые биты"), this);
    m_readableLabel = new QLabel(tr("Опрашивать"), this);
    m_writableLabel = new QLabel(tr("Разрешить запись"), this);
    m_maxValueLabel = new QLabel(tr("Макс. значение"), this);
    m_minValueLabel = new QLabel(tr("Мин. значение"), this);
    m_euLabel = new QLabel(tr("Единицы измерения"), this);

    m_nameLineEdit = new QLineEdit(this);
    m_descriptionTextEdit = new QTextEdit(this);
    m_addressSpin = new QSpinBox(this);
    m_dataTypeCombo = new QComboBox(this);
    m_byteOrderCombo = new QComboBox(this);
    m_firstBitSpin = new QSpinBox(this);
    m_lastBitSpin = new QSpinBox(this);
    m_readableCheck = new QCheckBox(this);
    m_writableCheck = new QCheckBox(this);
    //ToDo: Макс/мин пока не делаю ибо предвидится переход на шаблоны
    m_maxValueSpin = new QSpinBox(this);
    m_minValueSpin = new QSpinBox(this);
    m_euLineEdit = new QLineEdit(this);
    m_okButton = new QPushButton(tr("Сохранить"), this);
    m_cancelButton = new QPushButton(tr("Отмена"), this);

    //////////////////////////////////////////Инициализация
    m_nameLineEdit->setText(variable->name());

    m_descriptionTextEdit->setText(variable->description());

    m_addressSpin->setRange(0, 65535);
    m_addressSpin->setValue(variable->address());

    //TODO: fix
//    QMetaEnum dataTypes = QMetaEnum::fromType<ModbusVariable::DataType>();
//    for (int i = 0; i < dataTypes.keyCount(); i++) {
//        m_dataTypeCombo->addItem(dataTypes.key(i));
//        if (variable->dataType() == ModbusVariable::DataType(dataTypes.value(i)))
//            m_dataTypeCombo->setCurrentIndex(i);
//    }

//    TODO: fix
//    connect(m_dataTypeCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this]() {
//       bool enable = QMetaEnum::fromType<ModbusVariable::DataType>().value(m_dataTypeCombo->currentIndex()) == IVariable::Register;
//       m_firstBitSpin->setEnabled(enable);
//       m_lastBitSpin->setEnabled(enable);
//    });

    QMetaEnum byteOrders = QMetaEnum::fromType<ModbusVariable::ByteOrder>();
    for(int i = 0; i < byteOrders.keyCount(); i++) {
        m_byteOrderCombo->addItem(byteOrders.key(i));
        if(variable->byteOrder() == ModbusVariable::ByteOrder(byteOrders.value(i)))
            m_byteOrderCombo->setCurrentIndex(i);
    }

    m_firstBitSpin->setRange(0, 15);
//    m_firstBitSpin->setValue(variable->startBit());

    m_lastBitSpin->setRange(0, 15);
//    m_lastBitSpin->setValue(variable->endBit());

    m_readableCheck->setChecked(variable->isReadable());

    m_writableCheck->setChecked(variable->isWritable());

    m_euLineEdit->setText(variable->measureUnit());

    connect(m_okButton, &QPushButton::clicked, this, &ModbusVariableUI::updateProperties);

    connect(m_cancelButton, &QPushButton::clicked, this, &QWidget::close);

    //////////////////////////////////////////Размещение
    QHBoxLayout *signBitsLayout = new QHBoxLayout();
    QLabel *fromLabel = new QLabel(tr("С"), this);
    QLabel *toLabel = new QLabel(tr("по"), this);
    signBitsLayout->addWidget(fromLabel);
    signBitsLayout->addWidget(m_firstBitSpin);
    signBitsLayout->addWidget(toLabel);
    signBitsLayout->addWidget(m_lastBitSpin);

    QFormLayout *editLayout = new QFormLayout();
    editLayout->addRow(m_nameLabel, m_nameLineEdit);
    editLayout->addRow(m_readableLabel, m_readableCheck);
    editLayout->addRow(m_addressLabel, m_addressSpin);
    editLayout->addRow(m_dataTypeLabel, m_dataTypeCombo);
    editLayout->addRow(m_byteOrderLabel, m_byteOrderCombo);
    editLayout->addRow(m_signBitsLabel, signBitsLayout);
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

ModbusVariableUI::~ModbusVariableUI()
{

}

void ModbusVariableUI::updateProperties()
{
//    TODO:
//    if (ModbusVariable::DataType(QMetaEnum::fromType<ModbusVariable::DataType>().value(m_dataTypeCombo->currentIndex())) == IVariable::Register &&
//            m_firstBitSpin->value() > m_lastBitSpin->value()) {
//        QMessageBox::warning(this, tr("Недопустимые параметры"), tr("Номер младшего значащего бита не может быть больше номера старшего значащего бита."));
//        return;
//    }
    m_variable->setName(m_nameLineEdit->text());
    m_variable->setReadable(m_readableCheck->isChecked());
    m_variable->setAddress(m_addressSpin->value());

    //TODO: fix
//    m_variable->setDataType(ModbusVariable::DataType(QMetaEnum::fromType<ModbusVariable::DataType>().value(m_dataTypeCombo->currentIndex())));
    m_variable->setByteOrder(ModbusVariable::ByteOrder(QMetaEnum::fromType<ModbusVariable::ByteOrder>().value(m_byteOrderCombo->currentIndex())));
    //min
    //max
    m_variable->setMeasureUnit(m_euLineEdit->text());
    m_variable->setWritable(m_writableCheck->isChecked());
    m_variable->setDescription(m_descriptionTextEdit->toPlainText());

    m_variable->getConfigNode()->setAttribute("name", m_variable->name());
    m_variable->getConfigNode()->setAttribute("readavle", m_variable->isReadable());
    m_variable->getConfigNode()->setAttribute("address", m_variable->address());

    //TODO: fix
//    m_variable->getConfigNode()->setAttribute("dataType", m_variable->dataType());
    m_variable->getConfigNode()->setAttribute("byteOrder", m_variable->byteOrder());
    //min
    //max
    m_variable->getConfigNode()->setAttribute("measureUnit", m_variable->measureUnit());
    m_variable->getConfigNode()->setAttribute("writable", m_variable->isWritable());
    m_variable->getConfigNode()->setAttribute("description", m_variable->description());
}
