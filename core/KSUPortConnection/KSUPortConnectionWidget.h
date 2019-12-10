#ifndef KSUPORTCONNECTIONWIDGET_H
#define KSUPORTCONNECTIONWIDGET_H

#include "Common/PrefixHeader.h"

class KSUPortConnectionWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString portName READ portName)
    Q_PROPERTY(QSerialPort::BaudRate baudRate READ baudRate)
    Q_PROPERTY(QSerialPort::DataBits dataBits READ dataBits)
    Q_PROPERTY(QSerialPort::StopBits stopBits READ stopBits)
    Q_PROPERTY(QSerialPort::Parity parity READ parity)
    Q_PROPERTY(QSerialPort::FlowControl flowControl READ flowControl)
    Q_PROPERTY(quint16 characterSpacing READ characterSpacing)

public:
    explicit KSUPortConnectionWidget(QWidget *parent = nullptr);
    virtual ~KSUPortConnectionWidget();

    QString portName() const;
    QSerialPort::BaudRate baudRate() const;
    QSerialPort::DataBits dataBits() const;
    QSerialPort::StopBits stopBits() const;
    QSerialPort::Parity parity() const;
    QSerialPort::FlowControl flowControl() const;
    quint16 characterSpacing() const;
    QString portParametersString() const;

private:
    QLabel *m_portNameLabel = nullptr;
    QLabel *m_baudRateLabel = nullptr;
    QLabel *m_defaultBaudRateLabel = nullptr;
    QLabel *m_dataBitsLabel = nullptr;
    QLabel *m_parityLabel = nullptr;
    QLabel *m_stopBitsLabel = nullptr;
    QLabel *m_flowControlLabel = nullptr;
    QLabel *m_charterSpacingLabel = nullptr;

    QComboBox *m_portNameComboBox = nullptr;
    QComboBox *m_baudRateComBox = nullptr;
    QComboBox *m_defaultBaudRateComBox = nullptr;
    QComboBox *m_dataBitsComboBox = nullptr;
    QComboBox *m_parityComBox = nullptr;
    QComboBox *m_stopBitsComBox = nullptr;
    QComboBox *m_flowControlComBox = nullptr;
    QSpinBox *m_charterSpacingSpinBox = nullptr;

public slots:
    void readSettings();
    void readSettingsWithCurrentBaudrate();
    void writeSettings();
    void setDefault();
    void disablePortName(bool off = true);
};

#endif // KSUPORTCONNECTIONWIDGET_H
