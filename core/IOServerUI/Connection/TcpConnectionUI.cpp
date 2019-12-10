#include "TcpConnectionUI.h"
#include "IOServer/nsConnection/TcpConnection.h"

TcpConnectionUI::TcpConnectionUI(TcpConnection *connection)
{
    setWindowTitle(tr("TCP подклюючение"));
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowModality(Qt::WindowModal);

    m_connection = connection;

    //////////////////////////////////////////Создание
    m_nameLabel = new QLabel(tr("Название"), this);
    m_addressLabel = new QLabel(tr("Адрес"), this);
    m_portLabel = new QLabel(tr("Порт"), this);
    m_descriptionLabel = new QLabel(tr("Описание"), this);

    m_nameLineEdit = new QLineEdit(this);
    m_hostNameLineEdit = new QLineEdit(this);
    m_portSpin = new QSpinBox(this);
    m_descriptionTextEdit = new QTextEdit(this);
    m_okButton = new QPushButton(tr("Сохранить"), this);

    //////////////////////////////////////////Инициализация
    m_nameLineEdit->setText(connection->name());

    m_hostNameLineEdit->setText(connection->hostName());
    m_hostNameLineEdit->setValidator(new QRegExpValidator(QRegExp("((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))(?![\\d])"), m_hostNameLineEdit));

    m_portSpin->setRange(0, 65535);
    m_portSpin->setValue(connection->portNumber());

    m_descriptionTextEdit->setText(connection->description());

    connect(m_okButton, &QPushButton::clicked, this, &TcpConnectionUI::updateProperties);

    //////////////////////////////////////////Размещение
    QFormLayout *mainLayout = new QFormLayout();
    mainLayout->addRow(m_nameLabel, m_nameLineEdit);
    mainLayout->addRow(m_addressLabel, m_hostNameLineEdit);
    mainLayout->addRow(m_portLabel, m_portSpin);
    mainLayout->addRow(m_descriptionLabel, m_descriptionTextEdit);
    mainLayout->addWidget(m_okButton);

    setLayout(mainLayout);
}

TcpConnectionUI::~TcpConnectionUI()
{

}

// TODO: Была мысль это сделать внутри коннекшена на сигналах и слотах. Но пока забиваю поле в поле
void TcpConnectionUI::updateProperties()
{
    m_connection->setName(m_nameLineEdit->text());
    m_connection->setHostName(m_hostNameLineEdit->text());
    m_connection->setPortNumber(m_portSpin->text().toUShort());
    m_connection->setDescription(m_descriptionTextEdit->toPlainText());

    m_connection->domElement().setAttribute("name", m_nameLineEdit->text());
    m_connection->domElement().setAttribute("hostName", m_hostNameLineEdit->text());
    m_connection->domElement().setAttribute("portNumber", m_portSpin->text());
    m_connection->domElement().setAttribute("description", m_descriptionTextEdit->toPlainText());
}
