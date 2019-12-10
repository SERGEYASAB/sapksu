#include "UpdateKsuWizard.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsNode/ModbusKsuNode.h"
#include "ManualTesting/ModbusKsu/ModbusKsuManualFullTesting/ModbusKsuManualFullTestingWidget.h"
#include "ManualTesting/Ksu735/ManualFullTesting/Ksu735ManualFullTestingWidget.h"
#include "Common/QMessageBox/ScriptDelayMessageBox/ProgressBarMessageBox.h"
#include "Logger/KsuLogWidget/KsuLogWidget.h"
#include "ClassReadBeforePage.h"
#include "IOServer/nsVariable/ModbusVariable.h"
#include "IOServer/nsVariable/Ksu735Variable.h"
#include "Authentication/SystemUser.h"
#include "DBConfig/DBConfig.h"
#include "ManualTesting/Ksu735/ManualFullTesting/Ksu735ManualFullTestingParametr.h"
#include "ReportManager/ReportConfigure.h"
#include "StartWindow/ActionControlWidgetsShow.h"

void UpdateKsuWizard::init()
{
    m_reportConfig = new ReportConfigure();

    m_readBeforePage = new ClassReadBeforePage;
    m_readBeforePage->setTitle("Сбор данных до обновления");
    m_readAfterPage = new ClassReadBeforePage;
    m_readAfterPage->setTitle("Сбор данных после обновления");
}

void UpdateKsuWizard::writeResult()
{
    if (m_aliases.count() != m_oldValues.count() || m_aliases.count() != m_newValues.count() || m_aliases.count() != m_names.count())
        return;
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    QString beforePedState = m_readBeforePage->getPedState();
    QString afterPedState = m_readAfterPage->getPedState();

    QString initString = QString("INSERT INTO `UpdateKsuReport` "
                                 "(datetime, user, ksuName, softwareVersion, aliasName, name, aliasOldValue, aliasNewValue, result) "
                                 "VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                         .arg(m_initTime.toString("yyyy-MM-dd hh:mm:ss"))
                         .arg(SystemUser::instance()->user())
                         .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                         .arg(m_versionBefore)
                         .arg("init")
                         .arg("init")
                         .arg(beforePedState)
                         .arg(afterPedState)
                         .arg("1");

    QString deinitString = QString("INSERT INTO `UpdateKsuReport` "
                                   "(datetime, user, ksuName, softwareVersion, aliasName, name, aliasOldValue, aliasNewValue, result, address) "
                                   "VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10');")
                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                           .arg(SystemUser::instance()->user())
                           .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                           .arg(m_versionAfter)
                           .arg("deinit")
                           .arg("deinit")
                           .arg("0")
                           .arg("0")
                           .arg("1")
                           .arg("0");

    db.transaction();
    query.exec(initString);

    for (int i = 0; i < m_aliases.count(); i++) {

        QString oldValue = m_oldValues.at(i).toString();
        QString newValue = m_newValues.at(i).toString();

        if (oldValue.contains(".", Qt::CaseInsensitive) || newValue.contains(".", Qt::CaseInsensitive)) {

            query.exec(QString("INSERT INTO `UpdateKsuReport` "
                               "(datetime, user, ksuName, softwareVersion, aliasName, name, aliasOldValue, aliasNewValue, result, address) "
                               "VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10');")
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                       .arg(SystemUser::instance()->user())
                       .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                       .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                       .arg(m_aliases.at(i))
                       .arg(m_names.at(i))
                       .arg(QString::number(m_oldValues.at(i).toFloat(), 'f', 3))
                       .arg(QString::number(m_newValues.at(i).toFloat(), 'f', 3))
                       .arg(QString::number(m_oldValues.at(i).toFloat(), 'f', 3) ==
                            QString::number(m_newValues.at(i).toFloat(), 'f', 3) &&
                            !m_oldValues.at(i).toString().isEmpty()  ? "1" : "0")
                       .arg(m_address.at(i)));
        } else {

            query.exec(QString("INSERT INTO `UpdateKsuReport` "
                               "(datetime, user, ksuName, softwareVersion, aliasName, name, aliasOldValue, aliasNewValue, result, address) "
                               "VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10');")
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                       .arg(SystemUser::instance()->user())
                       .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                       .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                       .arg(m_aliases.at(i))
                       .arg(m_names.at(i))
                       .arg(oldValue)
                       .arg(newValue)
                       .arg(oldValue == newValue && !m_oldValues.at(i).toString().isEmpty()  ? "1" : "0")
                       .arg(m_address.at(i)));
        }
    }
    query.exec(deinitString);
    db.commit();
}

bool UpdateKsuWizard::cancelButtonClicked()
{
    QMessageBox msgBox;

    msgBox.setWindowTitle("Завершение работы");
    msgBox.setText("Вы действительно хотите завершить тестирование на соответствие параметров КСУ после перепрограммирования?");

    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
    if (layout)
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.setMinimumWidth(400);
    msgBox.setDefaultButton(QMessageBox::No);

    QPushButton *okButton = msgBox.addButton("Да", QMessageBox::AcceptRole);
    QPushButton *noButton = msgBox.addButton("Нет", QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == okButton) {
        m_readBeforePage->cancelTesting();
        return true;
    }

    return false;
}

//void UpdateKsuWizard::showReportWindow()
//{
//    setWindowState(Qt::WindowMinimized);

//    ActionControlWidgetsShow::showAdjustWidget(this, m_reportConfig, QSize(800, 600));
//}

void UpdateKsuWizard::printReport()
{
    QDateTime to = QDateTime::currentDateTime();
    m_reportConfig->previewReport(ReportConfigure::KsuUpdateReport, m_initTime, to);
}

void UpdateKsuWizard::printToPdf()
{
    QDateTime to = QDateTime::currentDateTime();
    m_reportConfig->printToPdfReport(ReportConfigure::KsuUpdateReport, m_initTime, to);
}

void UpdateKsuWizard::closeEvent(QCloseEvent *event)
{
    if (cancelButtonClicked())
        event->accept();
    else
        event->ignore();
}

void UpdateKsuWizard::reject()
{
    close();
}

UpdateKsuWizard::UpdateKsuWizard(QObject *parent)
{
    init();
    addPage(introPage());
    addPage(m_readBeforePage);
    addPage(updatePage());
    addPage(m_readAfterPage);
    addPage(finalPage());
    setOptions(QWizard::NoCancelButtonOnLastPage);
    setButtonLayout(QList<QWizard::WizardButton>()<<QWizard::Stretch<<QWizard::NextButton<<QWizard::CancelButton<<QWizard::FinishButton);

    connect(m_readBeforePage, &ClassReadBeforePage::started, [this]()
    {
        m_initTime = QDateTime::currentDateTime();
    });

    connect(m_readBeforePage, &ClassReadBeforePage::finished, this, &UpdateKsuWizard::fillValuesBefore);
    connect(m_readAfterPage, &ClassReadBeforePage::finished, this, &UpdateKsuWizard::fillValuesAfter);
}

QWizardPage *UpdateKsuWizard::introPage()
{
    QWizardPage *page = new QWizardPage(this);
    page->setTitle("Версия прошивки КСУ");
    page->setSubTitle("Пожалуйста проверьте текущую версию прошивки и классификатор параметров");
    page->setButtonText(QWizard::NextButton, "Далее");
    page->setButtonText(QWizard::CancelButton, "Отмена");
    page->setButtonText(QWizard::BackButton, "Назад");

    QLabel *klassificator = new QLabel("Классификатор:");
    QLineEdit *klassificatorLineEdit = new QLineEdit;
    klassificatorLineEdit->setText(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion());

    QLabel *ksuVersion = new QLabel("Версия ПО КСУ:");
    QLineEdit *ksuVersionLineEdit = new QLineEdit;
    ksuVersionLineEdit->setText(IOServer::instance()->getTestHistoryReport()->versionPOKSULineEdit());

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(klassificator, 0, 0);
    layout->addWidget(klassificatorLineEdit, 0, 1);
    layout->addWidget(ksuVersion, 1, 0);
    layout->addWidget(ksuVersionLineEdit, 1, 1);
    page->setLayout(layout);
    return page;
}

QWizardPage *UpdateKsuWizard::finalPage()
{
    QWizardPage *page = new QWizardPage(this);
    page->setTitle("Завершение тестирования");
    page->setSubTitle("Тестирование на соответствие параметров КСУ после перепрограммирования завершено. Результаты тестирования были сохранены в базе данных. Вы можете просмотреть и распечатать очет о результатах теста, нажав на кнопку ниже.");
    page->setButtonText(QWizard::FinishButton, "Завершить");

    //    QPushButton *reportWindowButton = new QPushButton(QIcon(":/Images/report.png"), tr("Открыть окно отчетов"));
    //    connect(reportWindowButton, &QPushButton::clicked, this, &UpdateKsuWizard::showReportWindow);

    QPushButton *printButton = new QPushButton(QIcon(":/Images/print.png"), tr("Печать отчета"));
    connect(printButton, &QPushButton::clicked, this, &UpdateKsuWizard::printReport);

    QPushButton *pdfButton = new QPushButton(QIcon(":/Images/pdf.png"), tr("Сохранить отчет"));
    connect(pdfButton, &QPushButton::clicked, this, &UpdateKsuWizard::printToPdf);

    QVBoxLayout *lay = new QVBoxLayout;
    //lay->addWidget(reportWindowButton);
    lay->addWidget(printButton);
    lay->addWidget(pdfButton);
    page->setLayout(lay);

    return page;
}

QWizardPage *UpdateKsuWizard::updatePage()
{
    QWizardPage *page = new QWizardPage(this);
    page->setTitle("Обновление прошивки контроллера");
    page->setSubTitle("Обновите прошивку контроллера и нажмите Далее");
    page->setButtonText(QWizard::NextButton, "Далее");
    page->setButtonText(QWizard::CancelButton, "Отмена");
    page->setButtonText(QWizard::BackButton, "Назад");
    return page;
}

void UpdateKsuWizard::fillValuesBefore(QList<IVariable *> params)
{
    m_aliases.clear();
    m_aliases.clear();
    m_oldValues.clear();
    for (auto i : params) {
        m_aliases.append(i->alias());
        m_names.append(i->name());
        m_oldValues.append(i->value());
        if (Ksu735Variable *ksu735Variable = qobject_cast<Ksu735Variable *>(i)) {
            m_address.append(QString::number(ksu735Variable->address()));
        } else if (ModbusVariable *modbusVariable = qobject_cast<ModbusVariable *>(i)) {
            m_address.append(QString::number(modbusVariable->address()));
        }

    }
    IVariable *ksuProgrammVersionVariable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Programm_version"));

    m_versionBefore = "";
    if (ksuProgrammVersionVariable != nullptr) {
        m_versionBefore = ksuProgrammVersionVariable->readValue().toString();
    }
}

void UpdateKsuWizard::fillValuesAfter(QList<IVariable *> params)
{
    m_newValues.clear();
    for (auto i : params) {
        m_newValues.append(i->value());
    }
    IVariable *ksuProgrammVersionVariable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_Programm_version"));

    m_versionAfter = "";
    if (ksuProgrammVersionVariable != nullptr) {
        m_versionAfter = ksuProgrammVersionVariable->readValue().toString();
    }
    writeResult();
}
