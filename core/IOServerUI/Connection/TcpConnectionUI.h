#ifndef TCPCONNECTIONUI_H
#define TCPCONNECTIONUI_H

#include <QtWidgets>

class TcpConnection;

class TcpConnectionUI : public QWidget
{
    Q_OBJECT

public:
    explicit TcpConnectionUI(TcpConnection *connection);
    virtual ~TcpConnectionUI();

private:
    TcpConnection *m_connection;

    QLabel *m_nameLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_addressLabel;
    QLabel *m_portLabel;

    QLineEdit *m_nameLineEdit;
    QLineEdit *m_hostNameLineEdit;
    QSpinBox *m_portSpin;
    QTextEdit *m_descriptionTextEdit;

    QPushButton *m_okButton;

private slots:
    void updateProperties();
};

#endif // TCPCONNECTIONUI_H
