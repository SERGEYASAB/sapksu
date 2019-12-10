#ifndef SERIALPORTCONNECTIONUI_H
#define SERIALPORTCONNECTIONUI_H

#include <QtWidgets>
#include <QSerialPortInfo>

class SerialPortConnection;

class SerialPortConnectionUI : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortConnectionUI(SerialPortConnection *connection);
    virtual ~SerialPortConnectionUI();

signals:
    void connectionSettingsSaved();

private slots:
    void updateProperties();

private:
    SerialPortConnection *m_connection = nullptr;

    QLabel *m_nameLabel = nullptr;
    QLabel *m_portLabel = nullptr;
    QLabel *m_baudrateLabel = nullptr;
    QLabel *m_dataLabel = nullptr;
    QLabel *m_parityLabel = nullptr;
    QLabel *m_stopBitsLabel = nullptr;
    QLabel *m_flowControlLabel = nullptr;
    QLabel *m_silenceLabel = nullptr;
    QLabel *m_descriptionLabel = nullptr;

    QLineEdit *m_nameLineEdit = nullptr;
    QComboBox *m_portCombo = nullptr;
    QComboBox *m_baudrateCombo = nullptr;
    QComboBox *m_dataCombo = nullptr;
    QComboBox *m_parityCombo = nullptr;
    QComboBox *m_stopBitsCombo = nullptr;
    QComboBox *m_flowControlCombo = nullptr;
    QSpinBox *m_silenceSpin = nullptr;
    QTextEdit *m_descriptionTextEdit = nullptr;

    QPushButton *m_saveButton = nullptr;
};
#endif // SERIALPORTCONNECTIONUI_H
