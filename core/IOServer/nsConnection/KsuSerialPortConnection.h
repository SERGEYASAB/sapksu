#ifndef KSUSERIALPORTCONNECTION_H
#define KSUSERIALPORTCONNECTION_H

#include "Common/PrefixHeader.h"
#include "IConnection.h"

class KsuSerialPortConnection : public IConnection
{
    Q_OBJECT
    Q_PROPERTY(QString portName READ portName WRITE setPortName)
    Q_PROPERTY(QSerialPort::BaudRate baudRate READ baudRate WRITE setBaudRate NOTIFY baudRateChanged)
    Q_PROPERTY(QSerialPort::DataBits dataBits READ dataBits WRITE setDataBits NOTIFY dataBitsChanged)
    Q_PROPERTY(QSerialPort::StopBits stopBits READ stopBits WRITE setStopBits NOTIFY stopBitsChanged)
    Q_PROPERTY(QSerialPort::Parity parity READ parity WRITE setParity NOTIFY parityChanged)
    Q_PROPERTY(QSerialPort::FlowControl flowControl READ flowControl WRITE setFlowControl NOTIFY flowControlChanged)
    Q_PROPERTY(quint16 characterReadTimeout READ characterReadTimeout WRITE setCharacterReadTimeout NOTIFY characterReadTimeoutChanged)

public:
    KsuSerialPortConnection(QObject *parent = nullptr);
    ~KsuSerialPortConnection();

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

    quint16 characterReadTimeout() const;
    void setCharacterReadTimeout(const quint16 &characterReadTimeout);

    void open() override final;
    void close() override final;
    bool isOpen() const override final;
    QByteArray sendData(const QByteArray &request, quint32 timeout) override;

public slots:
    void updatePortSettingsFromAppSettings();

signals:
    void baudRateChanged(qint32 baudRate);
    void dataBitsChanged(QSerialPort::DataBits dataBits);
    void stopBitsChanged(QSerialPort::StopBits stopBits);
    void parityChanged(QSerialPort::Parity parity);
    void flowControlChanged(QSerialPort::FlowControl flow);
    void characterReadTimeoutChanged(quint16 characterReadTimeout);

    void errorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort *m_port = nullptr;
    mutable QMutex *m_mutex = nullptr;

    quint16 m_characterReadTimeout = 0;
};

#endif // KSUSERIALPORTCONNECTION_H
