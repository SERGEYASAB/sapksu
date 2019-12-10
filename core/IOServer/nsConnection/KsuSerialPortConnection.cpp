#include "KsuSerialPortConnection.h"
#include "ManualTesting/Common/InterferenceSender.h"
#include "AppSettings/AppSettings.h"

KsuSerialPortConnection::KsuSerialPortConnection(QObject *parent) :
    IConnection(parent)
{
    m_port = new QSerialPort(this);
    m_mutex = new QMutex(QMutex::Recursive);

    connect(m_port, &QSerialPort::baudRateChanged, this, &KsuSerialPortConnection::baudRateChanged);
    connect(m_port, &QSerialPort::dataBitsChanged, this, &KsuSerialPortConnection::dataBitsChanged);
    connect(m_port, &QSerialPort::stopBitsChanged, this, &KsuSerialPortConnection::stopBitsChanged);
    connect(m_port, &QSerialPort::parityChanged, this, &KsuSerialPortConnection::parityChanged);
    connect(m_port, &QSerialPort::flowControlChanged, this, &KsuSerialPortConnection::flowControlChanged);
    // TODO: FIX it not worked on Linux
    //    connect(m_port, &QSerialPort::error, this, &KsuSerialPortConnection::errorOccurred);
}

KsuSerialPortConnection::~KsuSerialPortConnection()
{
    delete m_port;
    delete m_mutex;
}

QString KsuSerialPortConnection::portName() const
{
    QMutexLocker locker(m_mutex);
    return m_port->portName();
}

void KsuSerialPortConnection::setPortName(const QString &portName)
{
    QMutexLocker locker(m_mutex);

    qDebug() << "setPortName" << m_port->openMode();

    if (m_port->isOpen())
        m_port->close();

    m_port->setPortName(portName);
}

QSerialPort::BaudRate KsuSerialPortConnection::baudRate() const
{
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker)

    return static_cast<QSerialPort::BaudRate>(m_port->baudRate());
}

void KsuSerialPortConnection::setBaudRate(const QSerialPort::BaudRate &baudRate)
{
    QMutexLocker locker(m_mutex);

    if (m_port->isOpen())
        m_port->close();

    m_port->setBaudRate(baudRate);
}

QSerialPort::DataBits KsuSerialPortConnection::dataBits() const
{
    QMutexLocker locker(m_mutex);
    return m_port->dataBits();
}

void KsuSerialPortConnection::setDataBits(const QSerialPort::DataBits &dataBits)
{
    QMutexLocker locker(m_mutex);
    if (m_port->isOpen())
        m_port->close();

    m_port->setDataBits(dataBits);
}

QSerialPort::StopBits KsuSerialPortConnection::stopBits() const
{
    QMutexLocker locker(m_mutex);
    return m_port->stopBits();
}

void KsuSerialPortConnection::setStopBits(const QSerialPort::StopBits &stopBits)
{
    QMutexLocker locker(m_mutex);
    if (m_port->isOpen())
        m_port->close();

    m_port->setStopBits(stopBits);
}

QSerialPort::Parity KsuSerialPortConnection::parity() const
{
    QMutexLocker locker(m_mutex);
    return m_port->parity();
}

void KsuSerialPortConnection::setParity(const QSerialPort::Parity &parity)
{
    QMutexLocker locker(m_mutex);
    if (m_port->isOpen())
        m_port->close();

    m_port->setParity(parity);
}

QSerialPort::FlowControl KsuSerialPortConnection::flowControl() const
{
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker)

    return m_port->flowControl();
}

void KsuSerialPortConnection::setFlowControl(const QSerialPort::FlowControl &flowControl)
{
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker)

    if (m_port->isOpen())
        m_port->close();

    m_port->setFlowControl(flowControl);
}

quint16 KsuSerialPortConnection::characterReadTimeout() const
{
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker)

    return m_characterReadTimeout;
}

void KsuSerialPortConnection::setCharacterReadTimeout(const quint16 &characterReadTimeout)
{
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker)

    if (m_characterReadTimeout != characterReadTimeout) {
        m_characterReadTimeout = characterReadTimeout;
        emit characterReadTimeoutChanged(m_characterReadTimeout);
    }
}

void KsuSerialPortConnection::open()
{
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker)

    if (m_port->isOpen())
        return;

    m_port->open(QIODevice::ReadWrite);
}

void KsuSerialPortConnection::close()
{
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker)

    if (m_port->isOpen())
        m_port->close();
}

bool KsuSerialPortConnection::isOpen() const
{
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker)

    return m_port->isOpen();
}

QByteArray KsuSerialPortConnection::sendData(const QByteArray &request, quint32 timeout)
{
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker)

    if (!m_port->isOpen())
        open();

    if (!m_port->isOpen())
        return QByteArray();

    m_port->clear();
    QByteArray sendingBytes = InterferenceSender::instance()->setInterference(request);
    m_port->write(sendingBytes);

    qDebug() << "sendData TX:" << sendingBytes.toHex().toUpper();

    QByteArray response = QByteArray();
    if (m_port->waitForReadyRead(timeout)) {
        if (m_characterReadTimeout == 0) { /**< если межсимвольный интервал 0 - то считываем все сразу из порта */
            response = m_port->readAll();
            qDebug() << "sendData RX:" << response.toHex().toUpper();
            return response;
        } else {
            QByteArray response = m_port->readAll();
            while(m_port->waitForReadyRead(m_characterReadTimeout)) {
                response += m_port->readAll();
            }
            qDebug() << "sendData RX:" << response.toHex().toUpper();
            return response;
        }
    } else {
        qDebug() << "sendData RX:" << response.toHex().toUpper();
        return response;
    }
}

void KsuSerialPortConnection::updatePortSettingsFromAppSettings()
{
    bool isPortOpened = m_port->isOpen();
    auto settings = AppSettings::settings();

    if (settings->childGroups().contains(QStringLiteral("KSUSerialPortSettings"))) {
        settings->beginGroup(QStringLiteral("KSUSerialPortSettings"));

        setProperty("name", settings->value(QStringLiteral("Name"), QStringLiteral("KSUSerialPort")));
        setProperty("portName", settings->value(QStringLiteral("PortName"), QStringLiteral("COM7")));

        QString baudRateString = settings->value(QStringLiteral("BaudRate"), QSerialPort::Baud9600).toString();

        int baudRate = QMetaEnum::fromType<QSerialPort::BaudRate>().keyToValue(baudRateString.toLatin1().data());
        if (baudRate != -1)
            setBaudRate(static_cast<QSerialPort::BaudRate>(baudRate));

        QString dataBitsString = settings->value(QStringLiteral("DataBits"), QSerialPort::Data8).toString();
        int dataBits = QMetaEnum::fromType<QSerialPort::DataBits>().keyToValue(dataBitsString.toLatin1().data());
        if (dataBits != -1)
            setDataBits(static_cast<QSerialPort::DataBits>(dataBits));

        QString parityString = settings->value(QStringLiteral("Parity"), QSerialPort::NoParity).toString();
        int parity = QMetaEnum::fromType<QSerialPort::Parity>().keyToValue(parityString.toLatin1().data());
        if (parity != -1)
            setParity(static_cast<QSerialPort::Parity>(parity));

        QString stopBitsString = settings->value(QStringLiteral("StopBits"), QSerialPort::TwoStop).toString();
        int stopBits = QMetaEnum::fromType<QSerialPort::StopBits>().keyToValue(stopBitsString.toLatin1().data());
        if (stopBits != -1)
            setStopBits(static_cast<QSerialPort::StopBits>(stopBits));

        QString flowControlString = settings->value(QStringLiteral("FlowControl"), QSerialPort::NoFlowControl).toString();
        int flowControl = QMetaEnum::fromType<QSerialPort::FlowControl>().keyToValue(flowControlString.toLatin1().data());
        if (flowControl != -1)
            setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl));

        setProperty("characterReadTimeout", settings->value(QStringLiteral("characterReadTimeout"), 100));

        settings->endGroup();

    } else {
        setName(QStringLiteral("KSUSerialPort"));
        setPortName("COM7");
        setBaudRate(QSerialPort::Baud9600);
        setDataBits(QSerialPort::Data8);
        setParity(QSerialPort::NoParity);
        setStopBits(QSerialPort::OneStop);
        setFlowControl(QSerialPort::NoFlowControl);
        setCharacterReadTimeout(100);

        auto settings = AppSettings::settings();

        settings->beginGroup(QStringLiteral("KSUSerialPortSettings"));

        settings->setValue(QStringLiteral("Name"), name());
        settings->setValue(QStringLiteral("PortName"), portName());

        const char *baudRate = QMetaEnum::fromType<QSerialPort::BaudRate>().valueToKey(m_port->baudRate());
        if (baudRate)
            settings->setValue(QStringLiteral("BaudRate"), QString(baudRate));

        const char *dataBits = QMetaEnum::fromType<QSerialPort::DataBits>().valueToKey(m_port->dataBits());
        if (dataBits)
            settings->setValue(QStringLiteral("DataBits"), QString(dataBits));

        const char *parity = QMetaEnum::fromType<QSerialPort::Parity>().valueToKey(m_port->parity());
        if (parity)
            settings->setValue(QStringLiteral("Parity"), QString(parity));

        const char *stopBits = QMetaEnum::fromType<QSerialPort::StopBits>().valueToKey(m_port->stopBits());
        if (stopBits)
            settings->setValue(QStringLiteral("StopBits"), QString(stopBits));

        const char *flowControl = QMetaEnum::fromType<QSerialPort::FlowControl>().valueToKey(m_port->flowControl());
        if (flowControl)
            settings->setValue(QStringLiteral("FlowControl"), QString(flowControl));

        settings->setValue(QStringLiteral("characterReadTimeout"), characterReadTimeout());
        settings->endGroup();
    }

    if (isPortOpened)
        open();
}
