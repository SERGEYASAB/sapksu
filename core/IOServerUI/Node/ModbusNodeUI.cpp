#include "ModbusNodeUI.h"
#include "IOServer/nsNode/INode.h"
#include "IOServer/nsNode/ModbusNode.h"

ModbusNodeUI::ModbusNodeUI(ModbusNode *node)
{
    setWindowTitle(tr("Параметры узла"));
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowModality(Qt::WindowModal);

    m_node = node;

    //////////////////////////////////////////Создание
    m_nameLabel = new QLabel(tr("Название"), this);
    m_descriptionLabel = new QLabel(tr("Описание"), this);
    m_readFunctionLabel = new QLabel(tr("Функция чтения"), this);
    m_writeFunctionLabel = new QLabel(tr("Функция записи"), this);
    m_addressLabel = new QLabel(tr("Адрес устройства"), this);
    m_periodLabel = new QLabel(tr("Интервал опроса"), this);
    m_groupRequestLabel = new QLabel(tr("Групповой запрос"), this);
    m_maxLenghtPduLabel = new QLabel(tr("Макс. длина PDU"), this);
    m_relayAddressLabel = new QLabel(tr("Ретрансляторы"), this);
    m_firstCommingAddressLabel = new QLabel(tr("Адреса разбиения"), this);
    m_timeoutOfRestoreLabel = new QLabel(tr("Таймаут восстановления"), this);
    m_readableLabel = new QLabel(tr("Опрашивать"), this);
    m_numberOfRetriesLabel = new QLabel(tr("Кол-во попыток опроса"), this);
    m_timeoutLabel = new QLabel(tr("Таймаут"));
    m_hollowRegistersCountLabel = new QLabel(tr("Чтение незначащих регистров"), this);

    m_nameLineEdit = new QLineEdit(this);
    m_descriptionTextEdit = new QTextEdit(this);
    m_readFunctionCombo = new QComboBox(this);
    m_writeFunctionCombo = new QComboBox(this);
    m_addressSpin = new QSpinBox(this);
    m_periodSpin = new QSpinBox(this);
    m_groupRequestCheck = new QCheckBox(this);
    m_maxLengthPduSpin = new QSpinBox(this);
    m_relayAddressLineEdit = new QLineEdit(this);
    m_firstCommingAddressLineEdit = new QLineEdit(this);
    m_timeoutOfRestoreSpin = new QSpinBox(this);
    m_readableCheck = new QCheckBox(this);
    m_numberOfRetriesSpin = new QSpinBox(this);
    m_timeoutSpin = new QSpinBox(this);
    m_hollowRegistersCountCheck = new QCheckBox(this);
    m_okButton = new QPushButton(tr("Сохранить"), this);

    //////////////////////////////////////////Инициализация
    m_nameLineEdit->setText(node->name());

    m_descriptionTextEdit->setText(node->description());

    QMetaEnum readFunctions = QMetaEnum::fromType<ModbusNode::ModbusReadFunction>();
    for(int i = 0; i < readFunctions.keyCount(); i++) {
        m_readFunctionCombo->addItem(/*QString::number(readFunctions.value(i)) + " - " + */readFunctions.key(i));
        if(node->readFunction() == ModbusNode::ModbusReadFunction(readFunctions.value(i)))
            m_readFunctionCombo->setCurrentIndex(i);
    }
    connect(m_readFunctionCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this]()
    {
        ModbusNode::ModbusReadFunction selectedFunction= ModbusNode::ModbusReadFunction(QMetaEnum::fromType<ModbusNode::ModbusReadFunction>().value(m_readFunctionCombo->currentIndex()));
        m_writeFunctionCombo->setDisabled(selectedFunction == ModbusNode::ReadDiscreteInputs ||
                                          selectedFunction == ModbusNode::ReadInputRegister);
    });

    QMetaEnum writeFunctions = QMetaEnum::fromType<ModbusNode::ModbusWriteFunction>();
    for(int i = 0; i < writeFunctions.keyCount(); i++) {
        m_writeFunctionCombo->addItem(/*QString::number(writeFunctions.value(i)) + " - " + */writeFunctions.key(i));
        if(node->writeFunction() == ModbusNode::ModbusWriteFunction(writeFunctions.value(i)))
            m_writeFunctionCombo->setCurrentIndex(i);
    }

    m_addressSpin->setRange(0, 255);
    m_addressSpin->setValue(node->address());

    m_periodSpin->setRange(0, 65535);
    m_periodSpin->setValue(node->period());

    m_groupRequestCheck->setChecked(node->groupRequest());

    m_maxLengthPduSpin->setRange(0, 248);
    m_maxLengthPduSpin->setValue(node->maxLengthPdu());
    //ToDo: Валидаторы на поля с перечислениями адресов

    m_relayAddressLineEdit->setText(node->relayAddresses());

    m_firstCommingAddressLineEdit->setText(node->firstCommingAddresses());

    m_timeoutOfRestoreSpin->setRange(0, LONG_MAX);
    m_timeoutOfRestoreSpin->setValue(node->timeoutOfRestore());

    m_readableCheck->setChecked(node->isReadable());

    m_numberOfRetriesSpin->setRange(0, 100);
    m_numberOfRetriesSpin->setValue(node->numberOfRetries());

    m_timeoutSpin->setRange(0, 100000);
    m_timeoutSpin->setValue(node->timeout());

    m_hollowRegistersCountCheck->setChecked(node->hollowRegistersCount());

    //Времянка. Я считаю, что свойства нужно делать не в табе, а отдельным окном (как в нашем ОРС сервере)
    connect(m_okButton, &QPushButton::clicked, this, &ModbusNodeUI::updateProperties);


    //////////////////////////////////////////Размещение
    QHBoxLayout *mainLayout = new QHBoxLayout();
    QFormLayout *commonParametersLayout = new QFormLayout();
    QFormLayout *requestParametersLayout = new QFormLayout();

    QGroupBox *commonParametersGroup = new QGroupBox();
    commonParametersGroup->setLayout(commonParametersLayout);
    commonParametersGroup->setTitle(tr("Параметры узла"));
    QGroupBox *requestParametersGroup = new QGroupBox();
    requestParametersGroup->setLayout(requestParametersLayout);
    requestParametersGroup->setTitle(tr("Параметры опроса"));

    setLayout(mainLayout);
    mainLayout->addWidget(commonParametersGroup);
    mainLayout->addWidget(requestParametersGroup);

    commonParametersLayout->addRow(m_nameLabel, m_nameLineEdit);
    commonParametersLayout->addRow(m_readableLabel, m_readableCheck);
    commonParametersLayout->addRow(m_addressLabel, m_addressSpin);
    commonParametersLayout->addRow(m_readFunctionLabel, m_readFunctionCombo);
    commonParametersLayout->addRow(m_writeFunctionLabel, m_writeFunctionCombo);
    commonParametersLayout->addRow(m_descriptionLabel, m_descriptionTextEdit);

    requestParametersLayout->addRow(m_periodLabel, m_periodSpin);
    requestParametersLayout->addRow(m_groupRequestLabel, m_groupRequestCheck);
    requestParametersLayout->addRow(m_maxLenghtPduLabel, m_maxLengthPduSpin);
    requestParametersLayout->addRow(m_relayAddressLabel, m_relayAddressLineEdit);
    requestParametersLayout->addRow(m_firstCommingAddressLabel, m_firstCommingAddressLineEdit);
    requestParametersLayout->addRow(m_timeoutOfRestoreLabel, m_timeoutOfRestoreSpin);
    requestParametersLayout->addRow(m_numberOfRetriesLabel, m_numberOfRetriesSpin);
    requestParametersLayout->addRow(m_timeoutLabel, m_timeoutSpin);
    requestParametersLayout->addRow(m_hollowRegistersCountLabel, m_hollowRegistersCountCheck);

    requestParametersLayout->addWidget(m_okButton);
}

ModbusNodeUI::~ModbusNodeUI()
{

}

void ModbusNodeUI::updateProperties()
{
    m_node->setName(m_nameLineEdit->text());
    m_node->setReadable(m_readableCheck->isChecked());
    m_node->setAddress(m_addressSpin->value());
    m_node->setReadFunction(ModbusNode::ModbusReadFunction(QMetaEnum::fromType<ModbusNode::ModbusReadFunction>().value(m_readFunctionCombo->currentIndex())));
    m_node->setWriteFunction(ModbusNode::ModbusWriteFunction(QMetaEnum::fromType<ModbusNode::ModbusWriteFunction>().value(m_writeFunctionCombo->currentIndex())));
    m_node->setDescription(m_descriptionTextEdit->toPlainText());
    m_node->setPeriod(m_periodSpin->value());
    m_node->setGroupRequest(m_groupRequestCheck->isChecked());
    m_node->setMaxLengthPdu(m_maxLengthPduSpin->value());
    m_node->setRelayAddresses(m_relayAddressLineEdit->text());
    m_node->setFirstCommingAddresses(m_firstCommingAddressLineEdit->text());
    m_node->setTimeoutOfRestore(m_timeoutOfRestoreSpin->value());
    m_node->setNumberOfRetries(m_numberOfRetriesSpin->value());
    m_node->setTimeout(m_timeoutSpin->value());
    m_node->setHollowRegistersCount(m_hollowRegistersCountCheck->isChecked() ? 255 : 0);

    m_node->getConfigNode().setAttribute("name", m_nameLineEdit->text());
    m_node->getConfigNode().setAttribute("address", m_addressSpin->value());
    m_node->getConfigNode().setAttribute("readFunction", m_readFunctionCombo->currentText());
    m_node->getConfigNode().setAttribute("writeFunction", m_writeFunctionCombo->currentText());
    m_node->getConfigNode().setAttribute("description", m_descriptionTextEdit->toPlainText());
    m_node->getConfigNode().setAttribute("period", m_periodSpin->value());
    m_node->getConfigNode().setAttribute("groupRequest", m_groupRequestCheck->isChecked());
    m_node->getConfigNode().setAttribute("maxLengthPdu", m_maxLengthPduSpin->value());
    m_node->getConfigNode().setAttribute("relayAddresses", m_relayAddressLineEdit->text());
    m_node->getConfigNode().setAttribute("firstComingAddresses", m_firstCommingAddressLineEdit->text());
    m_node->getConfigNode().setAttribute("timeoutOfRestore", m_timeoutOfRestoreSpin->value());
    m_node->getConfigNode().setAttribute("numberOfRetries", m_numberOfRetriesSpin->value());
    m_node->getConfigNode().setAttribute("timeout", m_timeoutSpin->value());
    m_node->getConfigNode().setAttribute("hollowRegistersCount", m_hollowRegistersCountCheck->isChecked() ? 255 : 0);
}
