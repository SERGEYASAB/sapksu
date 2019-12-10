#ifndef SERIALPORTCONNECTION_H
#define SERIALPORTCONNECTION_H

#include "IOServer/nsConnection/IConnection.h"

#include <QObject>
#include <QSerialPort>

class SerialPortConnection : public IConnection
{
    Q_OBJECT
    Q_PROPERTY(QString portName READ portName WRITE setPortName)
    Q_PROPERTY(QSerialPort::BaudRate baudRate READ baudRate WRITE setBaudRate)
    Q_PROPERTY(QSerialPort::DataBits dataBits READ dataBits WRITE setDataBits)
    Q_PROPERTY(QSerialPort::StopBits stopBits READ stopBits WRITE setStopBits)
    Q_PROPERTY(QSerialPort::Parity parity READ parity WRITE setParity)
    Q_PROPERTY(QSerialPort::FlowControl flowControl READ flowControl WRITE setFlowControl)
    Q_PROPERTY(quint16 characterReadTimeout READ characterReadTimeout WRITE setCharacterReadTimeout)

public:
    explicit SerialPortConnection(QObject *parent = nullptr);
    explicit SerialPortConnection(QDomElement &domElement, QObject *parent = nullptr);
    ~SerialPortConnection() override;

    QString portName() const;
    void setPortName(const QString &portName);

    QSerialPort::BaudRate baudRate() const;
    void setBaudRate(const QSerialPort::BaudRate &baudRate);

    QSerialPort::DataBits dataBits() const;
    void setDataBits(const QSerialPort::DataBits &dataBits);

    QSerialPort::StopBits stopBits() const;
    void setStopBits(const QSerialPort::StopBits &stopBits);

    QSerialPort::Parity parity() const;
    void setParity(const QSerialPort::Parity &parity);

    QSerialPort::FlowControl flowControl() const;
    void setFlowControl(const QSerialPort::FlowControl &flowControl);

    void open() override final;
    void close() override final;
    bool isOpen() const override final;

    QByteArray sendData(const QByteArray &request, quint32 timeout) override;

    quint16 characterReadTimeout() const;
    void setCharacterReadTimeout(const quint16 &characterReadTimeout);

private slots:
    void handleError(QSerialPort::SerialPortError error) const;

private:
    QSerialPort *_port = nullptr;
    QString _portName;
    QSerialPort::BaudRate _baudRate;
    QSerialPort::DataBits _dataBits;
    QSerialPort::StopBits _stopBits;
    QSerialPort::Parity _parity;
    QSerialPort::FlowControl _flowControl;
    QSerialPort::SerialPortError _error;  
    quint16 _characterReadTimeout;
};

#endif // SERIALPORTCONNECTION_H
