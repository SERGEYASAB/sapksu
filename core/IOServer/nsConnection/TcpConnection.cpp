#include "IOServer/nsConnection/TcpConnection.h"
#include "IOServer/nsTask/IDataTask.h"

#include <QDebug>

TcpConnection::TcpConnection(QDomElement &domElement, QObject *parent)
    : IConnection(domElement, parent)
{
    _socket = new QTcpSocket(this);

    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleError(QAbstractSocket::SocketError)));

    connect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(handleSocketState(QAbstractSocket::SocketState)));

}

TcpConnection::~TcpConnection()
{
    _socket->disconnectFromHost();
}

QTcpSocket *TcpConnection::socket() const
{
    return _socket;
}


quint16 TcpConnection::portNumber() const
{
    return _portNumber;
}

void TcpConnection::setPortNumber(quint16 portNumber)
{
    _portNumber = portNumber;
}

QString TcpConnection::hostName() const
{
    return _hostName;
}

void TcpConnection::setHostName(const QString &hostName)
{
    _hostName = hostName;
}

void TcpConnection::open()
{
    if (_socket->state() == QAbstractSocket::ClosingState ||
            _socket->state() == QAbstractSocket::UnconnectedState) {
        _socket->connectToHost(_hostName, _portNumber);
        bool connected = _socket->waitForConnected(3000);
    }
}

void TcpConnection::close()
{
    if (_socket->state() != QAbstractSocket::ClosingState &&
            _socket->state() != QAbstractSocket::UnconnectedState) {
        _socket->disconnectFromHost();
    }
}

bool TcpConnection::isOpen() const
{
    return _socket->isOpen();
}

QByteArray TcpConnection::sendData(const QByteArray &request, quint32 timeout)
{
    open();

    if (socket()->state() != QTcpSocket::ConnectedState) {
        return QByteArray();
    }

    _socket->flush();
    _socket->write(request);

    // TODO процесс не бокируется в потоке!
    if (_socket->waitForReadyRead(timeout)) {
        return _socket->readAll();
    } else {
        return QByteArray();
    }
}

void TcpConnection::handleError(QAbstractSocket::SocketError error) const
{
    Q_UNUSED(error)
//    qInfo() << error << _hostName;
}

void TcpConnection::handleSocketState(QAbstractSocket::SocketState socketState) const
{
    Q_UNUSED(socketState)
//    qInfo() << socketState << "hostName:" << _hostName << "portNumber:" << _portNumber;
}
