#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include "IOServer/nsConnection/IConnection.h"

#include <QAbstractSocket>
#include <QTcpSocket>

class QTcpSocket;

class TcpConnection : public IConnection
{
    Q_OBJECT
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName)
    Q_PROPERTY(quint16 portNumber READ portNumber WRITE setPortNumber)

public:
    explicit TcpConnection(QDomElement &domElement, QObject *parent = Q_NULLPTR);
    ~TcpConnection() override;

    QTcpSocket *socket() const;

    QString hostName() const;
    void setHostName(const QString &hostName);

    quint16 portNumber() const;
    void setPortNumber(quint16 portNumber);

    void open() override final;
    void close() override final;
    bool isOpen() const override final;

    QByteArray sendData(const QByteArray &request, quint32 timeout) override final;

private slots:
    void handleError(QAbstractSocket::SocketError error) const;
    void handleSocketState(QAbstractSocket::SocketState socketState) const;

private:
    QTcpSocket *_socket;
    QString _hostName;
    quint16 _portNumber;
};

#endif // TCPCONNECTION_H
