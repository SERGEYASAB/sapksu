#include "SerialPortConnectionUI.h"
#include "IOServer/nsConnection/SerialPortConnection.h"

SerialPortConnectionUI::SerialPortConnectionUI(SerialPortConnection *connection)
{
    setWindowTitle(tr("COM подключение"));
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowModality(Qt::WindowModal);

    m_connection = connection;

    //////////////////////////////////////////Создание
    m_nameLabel = new QLabel(tr("Название"), this);
    m_portLabel = new QLabel(tr("Порт"), this);
    m_baudrateLabel = new QLabel(tr("Скорость"), this);
    m_dataLabel = new QLabel(tr("Данные"), this);
    m_parityLabel = new QLabel(tr("Паритет"), this);
    m_stopBitsLabel = new QLabel(tr("Стоп биты"), this);
    m_flowControlLabel = new QLabel(tr("Контроль"), this);
    m_silenceLabel = new QLabel(tr("Межсимвольный интервал"), this);
    m_descriptionLabel = new QLabel(tr("Описание"), this);

    m_nameLineEdit = new QLineEdit(this);
    m_portCombo = new QComboBox(this);
    m_baudrateCombo = new QComboBox(this);
    m_dataCombo = new QComboBox(this);
    m_parityCombo = new QComboBox(this);
    m_stopBitsCombo = new QComboBox(this);
    m_flowControlCombo = new QComboBox(this);
    m_silenceSpin = new QSpinBox(this);
    m_descriptionTextEdit = new QTextEdit(this);
    m_saveButton = new QPushButton(tr("Сохранить"), this);

    //////////////////////////////////////////Инициализация
    m_nameLineEdit->setText(connection->name());

    QStringList ports;
    for (auto port : QSerialPortInfo::availablePorts())
        ports << port.portName();

    ports.sort();
    m_portCombo->addItems(ports);
    m_portCombo->setCurrentText(m_connection->portName());

    QMetaEnum baudrates = QMetaEnum::fromType<QSerialPort::BaudRate>();
    for(int i = 0; i < baudrates.keyCount() - 1; i++)
    {
        QString str = QString(baudrates.key(i)).mid(4, 15);
        m_baudrateCombo->addItem(str);
        if(m_connection->baudRate() == QSerialPort::BaudRate(baudrates.value(i)))
            m_baudrateCombo->setCurrentIndex(i);
    }

    m_dataCombo->addItems(QStringList()<<"5"<<"6"<<"7"<<"8");
    QMetaEnum dataBits = QMetaEnum::fromType<QSerialPort::DataBits>();
    for(int i = 0; i < dataBits.keyCount() - 1; i++)
        if(m_connection->dataBits() == QSerialPort::DataBits(dataBits.value(i)))
            m_dataCombo->setCurrentIndex(i);

    m_parityCombo->addItems(QStringList()<<tr("Нет")<<tr("Четный")<<tr("Нечетный")<<tr("Пробел")<<tr("Метка"));
    QMetaEnum parity = QMetaEnum::fromType<QSerialPort::Parity>();
    for(int i = 0; i < parity.keyCount() - 1; i++)
        if(m_connection->parity() == QSerialPort::Parity(parity.value(i)))
            m_parityCombo->setCurrentIndex(i);

    m_stopBitsCombo->addItems(QStringList()<<"1"<<"1.5"<<"2");
    QMetaEnum stopBits = QMetaEnum::fromType<QSerialPort::StopBits>();
    for(int i = 0; i < stopBits.keyCount() - 1; i++)
        if(m_connection->stopBits() == QSerialPort::StopBits(stopBits.value(i)))
            m_stopBitsCombo->setCurrentIndex(i);

    m_flowControlCombo->addItems(QStringList()<<tr("Нет")<<tr("Аппаратный")<<tr("Программный"));
    QMetaEnum flowControl = QMetaEnum::fromType<QSerialPort::FlowControl>();
    for(int i = 0; i < flowControl.keyCount() - 1; i++)
        if(m_connection->flowControl() == QSerialPort::FlowControl(flowControl.value(i)))
            m_flowControlCombo->setCurrentIndex(i);

    m_silenceSpin->setRange(0, 500);
    m_silenceSpin->setValue(m_connection->characterReadTimeout());

    m_descriptionTextEdit->setText(connection->description());

    connect(m_saveButton, &QPushButton::clicked, this, &SerialPortConnectionUI::updateProperties);


    //////////////////////////////////////////Размещение
    QFormLayout *mainLayout = new QFormLayout();
    mainLayout->addRow(m_nameLabel, m_nameLineEdit);
    mainLayout->addRow(m_portLabel, m_portCombo);
    mainLayout->addRow(m_baudrateLabel, m_baudrateCombo);
    mainLayout->addRow(m_dataLabel, m_dataCombo);
    mainLayout->addRow(m_parityLabel, m_parityCombo);
    mainLayout->addRow(m_stopBitsLabel, m_stopBitsCombo);
    mainLayout->addRow(m_flowControlLabel, m_flowControlCombo);
    mainLayout->addRow(m_silenceLabel, m_silenceSpin);
    mainLayout->addRow(m_descriptionLabel, m_descriptionTextEdit);
    mainLayout->addWidget(m_saveButton);

    setLayout(mainLayout);
}

SerialPortConnectionUI::~SerialPortConnectionUI()
{

}

// TODO: Была мысль это сделать внутри коннекшена на сигналах и слотах. Но пока забиваю поле в поле
void SerialPortConnectionUI::updateProperties()
{
    m_connection->setName(m_nameLineEdit->text());
    m_connection->setPortName(m_portCombo->currentText());
    m_connection->setBaudRate(QSerialPort::BaudRate(QMetaEnum::fromType<QSerialPort::BaudRate>().
                                                    value(m_baudrateCombo->currentIndex())));
    m_connection->setDataBits(QSerialPort::DataBits(QMetaEnum::fromType<QSerialPort::DataBits>().
                                                    value(m_dataCombo->currentIndex())));
    m_connection->setParity(QSerialPort::Parity(QMetaEnum::fromType<QSerialPort::Parity>().
                                                value(m_parityCombo->currentIndex())));
    m_connection->setStopBits(QSerialPort::StopBits(QMetaEnum::fromType<QSerialPort::StopBits>().
                                                    value(m_stopBitsCombo->currentIndex())));
    m_connection->setFlowControl(QSerialPort::FlowControl(QMetaEnum::fromType<QSerialPort::FlowControl>().
                                                          value(m_flowControlCombo->currentIndex())));
    m_connection->setCharacterReadTimeout(quint16(m_silenceSpin->value()));
    m_connection->setDescription(m_descriptionTextEdit->toPlainText());

    m_connection->domElement().setAttribute("name", m_nameLineEdit->text());
    m_connection->domElement().setAttribute("portName", m_portCombo->currentText());
    m_connection->domElement().setAttribute("baudRate", QMetaEnum::fromType<QSerialPort::BaudRate>().
                                            valueToKey(m_connection->baudRate()));
    m_connection->domElement().setAttribute("dataBits", QMetaEnum::fromType<QSerialPort::DataBits>().
                                            valueToKey(m_connection->dataBits()));
    m_connection->domElement().setAttribute("parity", QMetaEnum::fromType<QSerialPort::Parity>().
                                            valueToKey(m_connection->parity()));
    m_connection->domElement().setAttribute("stopBits", QMetaEnum::fromType<QSerialPort::StopBits>().
                                            valueToKey(m_connection->stopBits()));
    m_connection->domElement().setAttribute("flowControl", QMetaEnum::fromType<QSerialPort::FlowControl>().valueToKey(m_connection->flowControl()));
    m_connection->domElement().setAttribute("characterReadTimeout", m_silenceSpin->value());
    m_connection->domElement().setAttribute("description", m_descriptionTextEdit->toPlainText());

    emit connectionSettingsSaved();
}
