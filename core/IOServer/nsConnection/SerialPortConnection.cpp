#include "SerialPortConnection.h"
#include "IOServer/nsTask/IDataTask.h"

#include <QDebug>
#include <QSerialPortInfo>
#include <QTime>
#include <QApplication>

SerialPortConnection::SerialPortConnection(QObject *parent) :
    IConnection(parent), _characterReadTimeout(0)
{
    _port = new QSerialPort(this);

    connect(_port, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(handleError(QSerialPort::SerialPortError)));
}

SerialPortConnection::SerialPortConnection(QDomElement &domElement, QObject *parent)
    : IConnection(domElement, parent), _characterReadTimeout(0)
{
    _port = new QSerialPort(this);

    connect(_port, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(handleError(QSerialPort::SerialPortError)));
}

SerialPortConnection::~SerialPortConnection()
{
    if(_port->isOpen())
        _port->close();
}

QString SerialPortConnection::portName() const
{
    return _portName;
}

void SerialPortConnection::setPortName(const QString &portName)
{
    _portName = portName;
}

QSerialPort::BaudRate SerialPortConnection::baudRate() const
{
    return _baudRate;
}

void SerialPortConnection::setBaudRate(const QSerialPort::BaudRate &baudRate)
{
    _baudRate = baudRate;
}

QSerialPort::DataBits SerialPortConnection::dataBits() const
{
    return _dataBits;
}

void SerialPortConnection::setDataBits(const QSerialPort::DataBits &dataBits)
{
    _dataBits = dataBits;
}

QSerialPort::StopBits SerialPortConnection::stopBits() const
{
    return _stopBits;
}

void SerialPortConnection::setStopBits(const QSerialPort::StopBits &stopBits)
{
    _stopBits = stopBits;
}

QSerialPort::Parity SerialPortConnection::parity() const
{
    return _parity;
}

void SerialPortConnection::setParity(const QSerialPort::Parity &parity)
{
    _parity = parity;
}

QSerialPort::FlowControl SerialPortConnection::flowControl() const
{
    return _flowControl;
}

void SerialPortConnection::setFlowControl(const QSerialPort::FlowControl &flowControl)
{
    _flowControl = flowControl;
}

void SerialPortConnection::open()
{
    if (_port->isOpen())
        _port->close();
    _port->setPortName(_portName);
    _port->setBaudRate(_baudRate);
    _port->setDataBits(_dataBits);
    _port->setParity(_parity);
    _port->setStopBits(_stopBits);
    _port->setFlowControl(_flowControl);
    _port->open(QIODevice::ReadWrite);

//    if(_port->isOpen())
//        qInfo() << QString(tr("Connected to %1 : %2, %3, %4, %5, %6")).
//                   arg(_portName).
//                   arg(_baudRate).
//                   arg(_dataBits).
//                   arg(_parity).
//                   arg(_stopBits).
//                   arg(_flowControl);
}

void SerialPortConnection::close()
{
    if(_port->isOpen())
        _port->close();

//    qInfo() << QString(tr("Disconnected %1")).arg(_portName);
}

bool SerialPortConnection::isOpen() const
{
    return _port->isOpen();
}

QByteArray SerialPortConnection::sendData(const QByteArray &request, quint32 timeout)
{
//    qDebug() << "SerialPortConnection::sendData" << portName() << _port->isOpen() << _port->errorString();

    if (!_port->isOpen())
        open();

    if (!_port->isOpen())
        return QByteArray();

    _port->clear();
    _port->write(request);

    QByteArray response = QByteArray();
    if (_port->waitForReadyRead(timeout)) {
        if (_characterReadTimeout == 0) { /**< если межсимвольный интервал 0 - то считываем все сразу из порта */
            response = _port->readAll();
            return response;
        } else {
            QByteArray response = _port->readAll();
            while(_port->waitForReadyRead(_characterReadTimeout)) {
                response += _port->readAll();
            }
            return response;
        }
    } else {
        return response;
    }

    //    emit task->receivedResponse(tx, _port->readAll());
}

void SerialPortConnection::handleError(QSerialPort::SerialPortError error) const
{
//    qInfo() << error << _portName;
}

quint16 SerialPortConnection::characterReadTimeout() const
{
    return _characterReadTimeout;
}

void SerialPortConnection::setCharacterReadTimeout(const quint16 &characterReadTimeout)
{
    _characterReadTimeout = characterReadTimeout;
}
