#include "KsuTestResultDialog.h"
#include "IOServer/IOServer.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "Authentication/SystemUser.h"
#include "DBConfig/DBConfig.h"

KsuTestResultDialog::KsuTestResultDialog(QWidget *parent) : QDialog(parent)
{
    m_mainLayout = new QVBoxLayout();

    m_resultLabel = new QLabel(tr("Результат:"));

    m_resultNames[Ok] = tr("Годен");
    m_resultNames[NotOk] = tr("Не годен");

    m_resultCombo = new QComboBox();
    m_resultCombo->setEditable(false);
    for (auto i : m_resultNames.values())
        m_resultCombo->addItem(i);

    m_dataLayout = new QFormLayout();
    m_dataLayout->addRow(m_resultLabel, m_resultCombo);

    m_saveButton = new QPushButton(tr("Сохранить"));
    connect(m_saveButton, &QPushButton::clicked, this, &KsuTestResultDialog::save);

    m_dontSaveButton = new QPushButton(tr("Не сохранять"));
    connect(m_dontSaveButton, &QPushButton::clicked, this, &KsuTestResultDialog::accept);

    m_cancelButton = new QPushButton(tr("Отмена"));
    connect(m_cancelButton, &QPushButton::clicked, this, &KsuTestResultDialog::reject);

    m_buttonsLayout = new QHBoxLayout;
    m_buttonsLayout->addWidget(m_saveButton);
    m_buttonsLayout->addWidget(m_dontSaveButton);
    m_buttonsLayout->addWidget(m_cancelButton);

    m_infoLabel = new QLabel(tr("Выберите действие."));
    m_mainLayout->addWidget(m_infoLabel);
    m_mainLayout->addLayout(m_dataLayout);
    m_mainLayout->addLayout(m_buttonsLayout);

    setLayout(m_mainLayout);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
}

void KsuTestResultDialog::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);
    QString initStr = QString("INSERT INTO `TestHistoryReport` "
                              "(function, user, ksuName, softwareVersion, kitName, testName, stepName, result, stepNumber, message) "
                              "VALUES('init', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                      .arg(SystemUser::instance()->user())
                      .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                      .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                      .arg(tr("Ручной режим"))
                      .arg(tr("Ручное тестирование"))
                      .arg("init")
                      .arg(true)
                      .arg(QString::number(0))
                      .arg("Номер протокола: " + IOServer::instance()->getTestHistoryReport()->protokolNumberLineEdit() +
                           "Тип СУ: " + IOServer::instance()->getTestHistoryReport()->typeSULineEdit() +
                           "Заводской номер СУ: " + IOServer::instance()->getTestHistoryReport()->serialNumberSULineEdit() +
                           "Год выпуска СУ: " + IOServer::instance()->getTestHistoryReport()->issueYearSULineEdit() +
                           "Инвентарный номер СУ: " + IOServer::instance()->getTestHistoryReport()->invNumberSULineEdit() +
                           "Тип КСУ: " + IOServer::instance()->getTestHistoryReport()->typeKSULineEdit() +
                           "Заводской номер КСУ: " + IOServer::instance()->getTestHistoryReport()->serialNumberKSULineEdit() +
                           "Версия ПО КСУ: " + IOServer::instance()->getTestHistoryReport()->versionPOKSULineEdit() +
                           "Вид ремонта СУ: " + IOServer::instance()->getTestHistoryReport()->kindOfRepairsLineEdit());

    QString testStr = QString("INSERT INTO `TestHistoryReport` "
                              "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, result, stepNumber) "
                              "VALUES('Ручной режим', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                      .arg(SystemUser::instance()->user())
                      .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                      .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                      .arg(tr("Ручной режим"))
                      .arg(tr("Ручной режим"))
                      .arg(tr("Ручное тестирование"))
                      .arg(tr("Ручной режим"))
                      .arg(m_resultCombo->currentText() == "Годен" ? "true" : "false")
                      .arg(QString::number(0));

    QString deinitStr = QString("INSERT INTO `TestHistoryReport` "
                                "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, result, stepNumber) "
                                "VALUES('deinit', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                        .arg(SystemUser::instance()->user())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                        .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                        .arg("Деинициализация")
                        .arg(tr("Ручной режим"))
                        .arg(tr("Ручное тестирование"))
                        .arg("deinit")
                        .arg(true)
                        .arg(QString::number(0));

    db.transaction();
    query.exec(initStr);
    query.exec(testStr);
    query.exec(deinitStr);
    db.commit();
    accept();
}
