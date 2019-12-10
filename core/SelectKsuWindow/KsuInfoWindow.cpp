#include "KsuInfoWindow.h"
#include "IOServer/IOServer.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "IOServer/nsVariable/IVariable.h"
#include "DBConfig/DBConfig.h"
#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsNode/ModbusKsuNode.h".h"

KsuInfoWindow::KsuInfoWindow(QWidget *parent) : QDialog(parent)
{
    QFormLayout *ksuInfoLayout = new QFormLayout;

    QLabel *m_protokolNumber = new QLabel("Номер протокола");
    QLabel *m_typeSU = new QLabel("Тип СУ");
    QLabel *m_serialNumberSU = new QLabel("Заводской номер СУ");
    QLabel *m_issueYearSU = new QLabel("Год выпуска СУ");
    QLabel *m_invNumberSU = new QLabel("Инвентарный номер СУ");
    QLabel *m_typeKSU = new QLabel("Тип КСУ");
    QLabel *m_serialNumberKSU = new QLabel("Заводской номер КСУ");
    QLabel *m_versionPOKSU = new QLabel("Версия ПО КСУ");
    QLabel *m_kindOfRepairs = new QLabel("Вид ремонта СУ");

    m_protokolNumberLineEdit = new QLineEdit(this);
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `SUTypes` "
                                "ORDER BY `SUName` ASC");

    QSqlQuery query(sqlString, db);
    m_typeSULineEdit = new QComboBox(this);
    m_typeSULineEdit->setEditable(true);
    while (query.next())
        m_typeSULineEdit->addItem(query.value("SUName").toString());
    m_serialNumberSULineEdit = new QLineEdit(this);
    m_issueYearSULineEdit = new QLineEdit(this);
    m_invNumberSULineEdit = new QLineEdit(this);

    sqlString = QString("SELECT * FROM `KsuTypes` "
                                "ORDER BY `ksuName` ASC");
    QSqlQuery query2(sqlString, db);

    m_typeKSULineEdit = new QComboBox(this);
    m_typeKSULineEdit->setEditable(true);
    while (query2.next())
        m_typeKSULineEdit->addItem(query2.value("ksuName").toString());

    m_serialNumberKSULineEdit = new QLineEdit(this);
    m_versionPOKSULineEdit = new QLineEdit(this);
    m_kindOfRepairsLineEdit = new QLineEdit(this);
    m_okButton = new QPushButton(tr("Ок"), this);
    m_readAgain = new QPushButton(tr("Перечитать"), this);

    connect(m_okButton, &QPushButton::clicked, this, &KsuInfoWindow::close);
    connect(m_readAgain, &QPushButton::clicked, this, &KsuInfoWindow::readAgain);

    ksuInfoLayout->addRow(m_protokolNumber, m_protokolNumberLineEdit);
    ksuInfoLayout->addRow(m_typeSU, m_typeSULineEdit);
    ksuInfoLayout->addRow(m_serialNumberSU, m_serialNumberSULineEdit);
    ksuInfoLayout->addRow(m_issueYearSU, m_issueYearSULineEdit);
    ksuInfoLayout->addRow(m_invNumberSU, m_invNumberSULineEdit);
    ksuInfoLayout->addRow(m_typeKSU, m_typeKSULineEdit);
    ksuInfoLayout->addRow(m_serialNumberKSU, m_serialNumberKSULineEdit);
    ksuInfoLayout->addRow(m_versionPOKSU, m_versionPOKSULineEdit);
    ksuInfoLayout->addRow(m_kindOfRepairs, m_kindOfRepairsLineEdit);
    ksuInfoLayout->addWidget(m_okButton);
    ksuInfoLayout->addWidget(m_readAgain);

    QTimer::singleShot(5000, this, SLOT(readAgain()));

    setLayout(ksuInfoLayout);
}

void KsuInfoWindow::save()
{
    IOServer::instance()->getTestHistoryReport()->setProtokolNumberLineEdit(m_protokolNumberLineEdit->text());
    IOServer::instance()->getTestHistoryReport()->setTypeSULineEdit(m_typeSULineEdit->currentText());
    IOServer::instance()->getTestHistoryReport()->setSerialNumberSULineEdit(m_serialNumberSULineEdit->text());
    IOServer::instance()->getTestHistoryReport()->setIssueYearSULineEdit(m_issueYearSULineEdit->text());
    IOServer::instance()->getTestHistoryReport()->setInvNumberSULineEdit(m_invNumberSULineEdit->text());
    IOServer::instance()->getTestHistoryReport()->setTypeKSULineEdit(m_typeKSULineEdit->currentText());
    IOServer::instance()->getTestHistoryReport()->setSerialNumberKSULineEdit(m_serialNumberKSULineEdit->text());
    IOServer::instance()->getTestHistoryReport()->setVersionPOKSULineEdit(m_versionPOKSULineEdit->text());
    IOServer::instance()->getTestHistoryReport()->setKindOfRepairsLineEdit(m_kindOfRepairsLineEdit->text());
}

void KsuInfoWindow::readAgain()
{
    // Инициализация параметров с КСУ перед началом самого тестирования

    // Дата время СУ

    IVariable *dateTimeSu1Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_manufacturedate_SU_01"));
    IVariable *dateTimeSu2Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_manufacturedate_SU_02"));

    if (Ksu735Node *ksu735Node = qobject_cast<Ksu735Node *>(IOServer::instance()->ksuNode())) {

        if (dateTimeSu1Variable != 0 && dateTimeSu2Variable != 0) {
            QString year = dateTimeSu1Variable->readValue().toString();
            QString month = dateTimeSu2Variable->readValue().toString();
            m_issueYearSULineEdit->setText(year + month);
        }

    } else if (ModbusKsuNode *modbusKsuNode = qobject_cast<ModbusKsuNode *>(IOServer::instance()->ksuNode())) {
        if (dateTimeSu1Variable != 0) {
            m_issueYearSULineEdit->setText(dateTimeSu1Variable->readValue().toDateTime().toString());
        }
    }

    // Тип КСУ
    IVariable *ksuTypeVariable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Type"));

    if (ksuTypeVariable != nullptr) {
        m_typeKSULineEdit->setCurrentText(ksuTypeVariable->readValue().toByteArray().data());
    }

    // Тип СУ
    IVariable *suTypeVariable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_type_SU"));

    if (ksuTypeVariable != nullptr) {
        m_typeSULineEdit->setCurrentText(suTypeVariable->readValue().toByteArray().data());
    }

    // Заводской номер КСУ

    IVariable *ksuSerialNumber0Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Serial_number_0"));
    IVariable *ksuSerialNumber1Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Serial_number_1"));
    IVariable *ksuSerialNumber2Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Serial_number_2"));
    IVariable *ksuSerialNumber3Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Serial_number_3"));

    if (Ksu735Node *ksu735Node = qobject_cast<Ksu735Node *>(IOServer::instance()->ksuNode())) {

        if (ksuSerialNumber0Variable != 0 && ksuSerialNumber1Variable != 0 && ksuSerialNumber2Variable != 0 && ksuSerialNumber3Variable != 0) {

            QList<int> values;
            QString string = QString();

            values.append(ksuSerialNumber0Variable->readValue().toInt());
            values.append(ksuSerialNumber1Variable->readValue().toInt());
            values.append(ksuSerialNumber2Variable->readValue().toInt());
            values.append(ksuSerialNumber3Variable->readValue().toInt());


            for (auto value : values) {

                union { quint16 value; char bytes[2];} valueToBytes;
                valueToBytes.value = value;

                auto decoder = QTextCodec::codecForName("cp866")->makeDecoder();
                QByteArray cp866Bytes;
                cp866Bytes.append(valueToBytes.bytes[1]);
                cp866Bytes.append(valueToBytes.bytes[0]);

                string.append(decoder->toUnicode(cp866Bytes));
            }

            m_serialNumberKSULineEdit->setText(string.remove(QRegExp("\\W")));

        }

    } else if (ModbusKsuNode *modbusKsuNode = qobject_cast<ModbusKsuNode *>(IOServer::instance()->ksuNode())) {
        if (ksuSerialNumber0Variable != 0) {
            m_serialNumberKSULineEdit->setText(ksuSerialNumber0Variable->readValue().toString());
        }
    }

    // Заводской номер СУ

    IVariable *suSerialNumber0Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Serial_number_SU_0"));
    IVariable *suSerialNumber1Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Serial_number_SU_1"));
    IVariable *suSerialNumber2Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Serial_number_SU_2"));
    IVariable *suSerialNumber3Variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Serial_number_SU_3"));

    if (Ksu735Node *ksu735Node = qobject_cast<Ksu735Node *>(IOServer::instance()->ksuNode())) {

        if (suSerialNumber0Variable != 0 && suSerialNumber1Variable != 0 && suSerialNumber2Variable != 0 && suSerialNumber3Variable != 0) {

            QList<int> values;
            QString string = QString();

            values.append(suSerialNumber0Variable->readValue().toInt());
            values.append(suSerialNumber1Variable->readValue().toInt());
            values.append(suSerialNumber2Variable->readValue().toInt());
            values.append(suSerialNumber3Variable->readValue().toInt());


            for (auto value : values) {

                union { quint16 value; char bytes[2];} valueToBytes;
                valueToBytes.value = value;

                auto decoder = QTextCodec::codecForName("cp866")->makeDecoder();
                QByteArray cp866Bytes;
                cp866Bytes.append(valueToBytes.bytes[1]);
                cp866Bytes.append(valueToBytes.bytes[0]);

                string.append(decoder->toUnicode(cp866Bytes));
            }

            m_serialNumberSULineEdit->setText(string.remove(QRegExp("\\W")));

        }


    } else if (ModbusKsuNode *modbusKsuNode = qobject_cast<ModbusKsuNode *>(IOServer::instance()->ksuNode())) {

        if (suSerialNumber0Variable != 0) {
            m_serialNumberSULineEdit->setText(ksuSerialNumber0Variable->readValue().toString());
        }
    }

    // Версия ПО
    IVariable *ksuProgrammVersionVariable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Programm_version"));

    if (ksuProgrammVersionVariable != nullptr) {
        m_versionPOKSULineEdit->setText(ksuProgrammVersionVariable->readValue().toString());
    }
}

void KsuInfoWindow::closeEvent(QCloseEvent *event)
{
    save();
    QDialog::closeEvent(event);
}
