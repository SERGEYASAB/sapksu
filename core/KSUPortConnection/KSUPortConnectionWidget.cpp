#include "KSUPortConnectionWidget.h"
#include "AppSettings/AppSettings.h"
#include "Authentication/SystemUser.h"

KSUPortConnectionWidget::KSUPortConnectionWidget(QWidget *parent) : QWidget(parent)
{
    // port
    m_portNameLabel = new QLabel(tr("Порт"), this);
    m_portNameComboBox = new QComboBox(this);
    QStringList ports;
    for (auto port : QSerialPortInfo::availablePorts())
        ports << port.portName();

    ports.sort();
    m_portNameComboBox->addItems(ports);

    // baudrate
    m_baudRateLabel = new QLabel(tr("Скорость"), this);
    m_baudRateComBox = new QComboBox(this);

    // defaultbaudrate
    m_defaultBaudRateLabel = new QLabel(tr("Скорость по умолчанию"), this);
    m_defaultBaudRateComBox = new QComboBox(this);

    if (SystemUser::instance()->access() < ACCESSLEVEL_ADMIN) {
        m_defaultBaudRateLabel->setVisible(false);
        m_defaultBaudRateComBox->setVisible(false);
    }

    QMetaEnum baudrates = QMetaEnum::fromType<QSerialPort::BaudRate>();
    for(int i = 0; i < baudrates.keyCount() - 1; i++)
    {
        QString str = QString(baudrates.key(i)).mid(4, 15);
        m_baudRateComBox->addItem(str);
        m_defaultBaudRateComBox->addItem(str);
        if (baudrates.value(i) == 9600) {
            m_baudRateComBox->setCurrentIndex(i);
            m_defaultBaudRateComBox->setCurrentIndex(i);
        }
    }

    // data
    m_dataBitsLabel = new QLabel(tr("Данные"), this);
    m_dataBitsComboBox = new QComboBox(this);
    m_dataBitsComboBox->addItems(QStringList()<<"5"<<"6"<<"7"<<"8");
    m_dataBitsComboBox->setCurrentIndex(3);

    // parity
    m_parityLabel = new QLabel(tr("Паритет"), this);
    m_parityComBox = new QComboBox(this);
    m_parityComBox->addItems(QStringList()<<tr("Нет")<<tr("Четный")<<tr("Нечетный")<<tr("Пробел")<<tr("Метка"));

    // stop bits
    m_stopBitsLabel = new QLabel(tr("Стоп биты"), this);
    m_stopBitsComBox = new QComboBox(this);
    m_stopBitsComBox->addItems(QStringList()<<"1"<<"1.5"<<"2");

    // flow control
    m_flowControlLabel = new QLabel(tr("Контроль"), this);
    m_flowControlComBox = new QComboBox(this);
    m_flowControlComBox->addItems(QStringList()<<tr("Нет")<<tr("Аппаратный")<<tr("Программный"));

    // silence spin
    m_charterSpacingLabel = new QLabel(tr("Межсимвольный интервал"), this);
    m_charterSpacingSpinBox = new QSpinBox(this);
    m_charterSpacingSpinBox->setRange(0, 200);
    m_charterSpacingSpinBox->setValue(100);

    QPushButton *defaultButton = new QPushButton("По умолчанию");

    connect(defaultButton, &QPushButton::clicked, this, &KSUPortConnectionWidget::setDefault);

    readSettings();

    QFormLayout *mainLayout = new QFormLayout();

    mainLayout->addRow(m_portNameLabel, m_portNameComboBox);
    mainLayout->addRow(m_baudRateLabel, m_baudRateComBox);
    mainLayout->addRow(m_defaultBaudRateLabel, m_defaultBaudRateComBox);
    mainLayout->addRow(m_dataBitsLabel, m_dataBitsComboBox);
    mainLayout->addRow(m_parityLabel, m_parityComBox);
    mainLayout->addRow(m_stopBitsLabel, m_stopBitsComBox);
    mainLayout->addRow(m_flowControlLabel, m_flowControlComBox);
    mainLayout->addRow(m_charterSpacingLabel, m_charterSpacingSpinBox);
    mainLayout->addRow(new QLabel(), defaultButton);

    setLayout(mainLayout);
}

KSUPortConnectionWidget::~KSUPortConnectionWidget()
{

}

QString KSUPortConnectionWidget::portName() const
{
    return m_portNameComboBox->currentText();
}

QSerialPort::BaudRate KSUPortConnectionWidget::baudRate() const
{
    int value = QMetaEnum::fromType<QSerialPort::BaudRate>().value(m_baudRateComBox->currentIndex());
    return static_cast<QSerialPort::BaudRate>(value);
}

QSerialPort::DataBits KSUPortConnectionWidget::dataBits() const
{
    int value = QMetaEnum::fromType<QSerialPort::DataBits>().value(m_dataBitsComboBox->currentIndex());
    return static_cast<QSerialPort::DataBits>(value);
}

QSerialPort::StopBits KSUPortConnectionWidget::stopBits() const
{
    int value = QMetaEnum::fromType<QSerialPort::StopBits>().value(m_stopBitsComBox->currentIndex());
    return static_cast<QSerialPort::StopBits>(value);
}

QSerialPort::Parity KSUPortConnectionWidget::parity() const
{
    int value = QMetaEnum::fromType<QSerialPort::Parity>().value(m_parityComBox->currentIndex());
    return static_cast<QSerialPort::Parity>(value);
}

QSerialPort::FlowControl KSUPortConnectionWidget::flowControl() const
{
    int value = QMetaEnum::fromType<QSerialPort::FlowControl>().value(m_flowControlComBox->currentIndex());
    return static_cast<QSerialPort::FlowControl>(value);
}

quint16 KSUPortConnectionWidget::characterSpacing() const
{
    return m_charterSpacingSpinBox->value();
}

QString KSUPortConnectionWidget::portParametersString() const
{
    return QString("%1, %2, %3, %4, %5")
            .arg(m_portNameComboBox->currentText())
            .arg(m_baudRateComBox->currentText())
            .arg(m_dataBitsComboBox->currentText())
            .arg(m_parityComBox->currentText())
            .arg(m_stopBitsComBox->currentText());
}

void KSUPortConnectionWidget::readSettings()
{
    auto settings = AppSettings::settings();

    if (settings->childGroups().contains(QStringLiteral("KSUSerialPortSettings"))) {

        settings->beginGroup(QStringLiteral("KSUSerialPortSettings"));

        m_portNameComboBox->setCurrentText(settings->value(QStringLiteral("PortName"), QStringLiteral("COM1")).toString());

        // QString baudRateString = settings->value(QStringLiteral("BaudRate"), QSerialPort::Baud9600).toString();
        // Ранее считывалось последнее записанное значение, по ТТ захотели иметь дефолтное значение и читать из него
        QString baudRateString = settings->value(QStringLiteral("DefaultBaudRate"), QSerialPort::Baud9600).toString();
        QMetaEnum baudRates = QMetaEnum::fromType<QSerialPort::BaudRate>();
        for (int i = 0; i < baudRates.keyCount() - 1; i++)
            if (baudRateString == baudRates.key(i))
                m_baudRateComBox->setCurrentIndex(i);

        QString defaultBaudRateString = settings->value(QStringLiteral("DefaultBaudRate"), QSerialPort::Baud9600).toString();
        QMetaEnum defaultBaudRates = QMetaEnum::fromType<QSerialPort::BaudRate>();
        for (int i = 0; i < defaultBaudRates.keyCount() - 1; i++)
            if (defaultBaudRateString == defaultBaudRates.key(i))
                m_defaultBaudRateComBox->setCurrentIndex(i);


        QString dataBitsString = settings->value(QStringLiteral("DataBits"), QSerialPort::Data8).toString();
        QMetaEnum dataBits = QMetaEnum::fromType<QSerialPort::DataBits>();
        for (int i = 0; i < dataBits.keyCount() - 1; i++)
            if (dataBitsString == dataBits.key(i))
                m_dataBitsComboBox->setCurrentIndex(i);

        QString parityString = settings->value(QStringLiteral("Parity"), QSerialPort::NoParity).toString();
        QMetaEnum parities = QMetaEnum::fromType<QSerialPort::Parity>();
        for (int i = 0; i < parities.keyCount() - 1; i++)
            if (parityString == parities.key(i))
                m_parityComBox->setCurrentIndex(i);

        QString stopBitsString = settings->value(QStringLiteral("StopBits"), QSerialPort::TwoStop).toString();
        QMetaEnum stopBits = QMetaEnum::fromType<QSerialPort::StopBits>();
        for (int i = 0; i < stopBits.keyCount() - 1; i++)
            if (stopBitsString == stopBits.key(i))
                m_stopBitsComBox->setCurrentIndex(i);

        QString flowControlString = settings->value(QStringLiteral("FlowControl"), QSerialPort::NoFlowControl).toString();
        QMetaEnum flowControls = QMetaEnum::fromType<QSerialPort::FlowControl>();
        for (int i = 0; i < flowControls.keyCount() - 1; i++)
            if (flowControlString == flowControls.key(i))
                m_flowControlComBox->setCurrentIndex(i);

        m_charterSpacingSpinBox->setValue(settings->value(QStringLiteral("CharacterReadTimeout"), 100).toInt());
    }
}

void KSUPortConnectionWidget::readSettingsWithCurrentBaudrate()
{
    auto settings = AppSettings::settings();

    if (settings->childGroups().contains(QStringLiteral("KSUSerialPortSettings"))) {

        settings->beginGroup(QStringLiteral("KSUSerialPortSettings"));

        QString baudRateString = settings->value(QStringLiteral("BaudRate"), QSerialPort::Baud9600).toString();
        QMetaEnum baudRates = QMetaEnum::fromType<QSerialPort::BaudRate>();
        for (int i = 0; i < baudRates.keyCount() - 1; i++)
            if (baudRateString == baudRates.key(i))
                m_baudRateComBox->setCurrentIndex(i);
    }

}

void KSUPortConnectionWidget::writeSettings()
{
    auto settings = AppSettings::settings();
    settings->beginGroup(QStringLiteral("KSUSerialPortSettings"));

    settings->setValue(QStringLiteral("PortName"), m_portNameComboBox->currentText());

    const char *baudRate = QMetaEnum::fromType<QSerialPort::BaudRate>().key(m_baudRateComBox->currentIndex());
    if (baudRate)
        settings->setValue(QStringLiteral("BaudRate"), QString(baudRate));

    const char *defaultBaudRate = QMetaEnum::fromType<QSerialPort::BaudRate>().key(m_defaultBaudRateComBox->currentIndex());
    if (defaultBaudRate)
        settings->setValue(QStringLiteral("DefaultBaudRate"), QString(defaultBaudRate));

    const char *dataBits = QMetaEnum::fromType<QSerialPort::DataBits>().key(m_dataBitsComboBox->currentIndex());
    if (dataBits)
        settings->setValue(QStringLiteral("DataBits"), QString(dataBits));

    const char *parity = QMetaEnum::fromType<QSerialPort::Parity>().key(m_parityComBox->currentIndex());
    if (parity)
        settings->setValue(QStringLiteral("Parity"), QString(parity));

    const char *stopBits = QMetaEnum::fromType<QSerialPort::StopBits>().key(m_stopBitsComBox->currentIndex());
    if (stopBits) settings->setValue(QStringLiteral("StopBits"), QString(stopBits));

    const char *flowControl = QMetaEnum::fromType<QSerialPort::FlowControl>().key(m_flowControlComBox->currentIndex());
    if (flowControl) settings->setValue(QStringLiteral("FlowControl"), QString(flowControl));

    settings->setValue(QStringLiteral("CharacterReadTimeout"), m_charterSpacingSpinBox->value());

    settings->endGroup();
    settings->sync();
}

void KSUPortConnectionWidget::setDefault()
{
#ifdef SAP_KSU
    m_portNameComboBox->setCurrentText(QString("COM7"));
#endif
#ifdef SAP_SU
    m_portNameComboBox->setCurrentText(QString("COM7"));
#endif
#ifdef MECHOS
    m_portNameComboBox->setCurrentText(QString("COM151"));
#endif

    m_baudRateComBox->setCurrentText(QString("9600"));
    m_dataBitsComboBox->setCurrentText(QString("8"));
    m_parityComBox->setCurrentText(QString("Нет"));
    m_stopBitsComBox->setCurrentText(QString("2"));
    m_flowControlComBox->setCurrentText(QString("Нет"));
    m_charterSpacingSpinBox->setValue(100);
}

void KSUPortConnectionWidget::disablePortName(bool off)
{
    m_portNameComboBox->setDisabled(off);
}


