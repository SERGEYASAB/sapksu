#include "ModbusLinkTedtingWidget.h"
#include "ui_ModbusLinkTedtingWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/ModbusKsuNode.h"
#include "IOServer/nsVariable/ModbusVariable.h"
#include "DBConfig/DBConfig.h"
#include "KSUPortConnection/KSUPortConnectionWidget.h"
#include "Authentication/SystemUser.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "ReportManager/OperatorClickReport.h"
#include "IOServer/IOServer.h"

ModbusLinkTedtingWidget::ModbusLinkTedtingWidget(ModbusKsuNode *node, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModbusLinkTedtingWidget),
    m_node(node)
{
    ui->setupUi(this);

    connect(ui->testButton, &QPushButton::clicked, this, &ModbusLinkTedtingWidget::test);
    connect(ui->requestButton, &QPushButton::clicked, this, &ModbusLinkTedtingWidget::request);
    connect(ui->clearButton, &QPushButton::clicked, this, &ModbusLinkTedtingWidget::clear);

    ui->progressBar->hide();
    ui->progressBar->setContentsMargins(QMargins());

    ui->progressLine->resize(24, 0);
    ui->progressLine->setContentsMargins(QMargins());
    ui->progressLine->setMinimumHeight(24);

    if (m_node) {
        ui->addressLabel->setText(QString::number(node->address()));

        QStringList aliases = {"KSU_Ia", "KSU_Ib", "KSU_Ic"};

        for (auto alias : aliases) {

            ModbusVariable *variable = static_cast<ModbusVariable *>(IOServer::instance()->variableByAlias(alias));
            VariableValue *variableValue = new VariableValue();
            variableValue->variable = variable;
            variableValue->value = QVariant();

            m_variableValues.append(variableValue);

        }

    } else {
        setEnabled(false);
    }
}

ModbusLinkTedtingWidget::~ModbusLinkTedtingWidget()
{
    delete ui;

    for (auto variableValue : m_variableValues) {
        delete variableValue;
    }
    m_variableValues.clear();
}

void ModbusLinkTedtingWidget::stopTesting()
{
    if (m_testing)
        test();
}

void ModbusLinkTedtingWidget::test()
{
    QWidgetList widgets;
    widgets << ui->requestCountSpinBox << ui->pauseSpinBox
            << ui->requestButton << ui->clearButton;

    m_testing = !m_testing;
    for (auto widget : widgets)
        widget->setEnabled(!m_testing);

    ui->progressBar->setVisible(m_testing);

    if (m_testing) {
        OperatorClickReport::instance()->write(QStringLiteral("Начато ручное тестирование КСУ по функции 0х10 (протокол Modbus)"));
        ui->testButton->setText("Стоп");
        m_testBeginDt.setDate(QDate::currentDate());
        m_testBeginDt.setTime(QTime::currentTime());
        testKsuConnection();
    } else {
        ui->testButton->setText("Тест");
        OperatorClickReport::instance()->write(QStringLiteral("Завершено ручное тестирование КСУ по функции 0х10 (протокол Modbus)"));
        QSqlQuery query(DBConfig::instance()->configDatabase());
        QDateTime currentDt;
        KSUPortConnectionWidget *portSettings = new KSUPortConnectionWidget(this);
        portSettings->readSettingsWithCurrentBaudrate();
        currentDt.setDate(QDate::currentDate());
        currentDt.setTime(QTime::currentTime());

        double q = (1.0 - double(m_rxCount) / double(m_txCount)) * 100;
        query.exec(QString("INSERT INTO `LinkStabilityReport`(begindatetime, enddatetime, user, ksuName, softwareVersion, portsettings, txcount, rxcount, errorcount, quality, spquality, result, message) "
                           "VALUES('%1', '%2', '%3', '%4', '%5', '%6', %7, %8, %9, %10, %11, '%12', '%13');")
                   .arg(m_testBeginDt.toString("yyyy-MM-dd hh:mm:ss"))
                   .arg(currentDt.toString("yyyy-MM-dd hh:mm:ss"))
                   .arg(SystemUser::instance()->user())
                   .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                   .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                   .arg(portSettings->portParametersString())
                   .arg(m_txCount)
                   .arg(m_rxCount)
                   .arg(m_errorCount)
                   .arg(QString::number(q))
                   .arg(QString::number(ui->spQualitySpin->value()))
                   .arg((1.0 - double(m_rxCount) / double(m_txCount)) < (double(ui->spQualitySpin->value()) / 100 ) ? "Годен" : "Не годен")
                   .arg("Номер протокола: " + IOServer::instance()->getTestHistoryReport()->protokolNumberLineEdit() +
                        "Тип СУ: " + IOServer::instance()->getTestHistoryReport()->typeSULineEdit() +
                        "Заводской номер СУ: " + IOServer::instance()->getTestHistoryReport()->serialNumberSULineEdit() +
                        "Год выпуска СУ: " + IOServer::instance()->getTestHistoryReport()->issueYearSULineEdit() +
                        "Инвентарный номер СУ: " + IOServer::instance()->getTestHistoryReport()->invNumberSULineEdit() +
                        "Тип КСУ: " + IOServer::instance()->getTestHistoryReport()->typeKSULineEdit() +
                        "Заводской номер КСУ: " + IOServer::instance()->getTestHistoryReport()->serialNumberKSULineEdit() +
                        "Версия ПО КСУ: " + IOServer::instance()->getTestHistoryReport()->versionPOKSULineEdit() +
                        "Вид ремонта СУ: " + IOServer::instance()->getTestHistoryReport()->kindOfRepairsLineEdit()));
                delete portSettings;
    }
}

void ModbusLinkTedtingWidget::request()
{
    //    if (m_node->readKsu0x10(INode::OperationTimingType::Sync)) {
    //        for (auto variableValue : m_variableValues) {
    //            variableValue->value = variableValue->variable->descriptionValue();
    //        }
    //    } else {
    //        QVariant errorDescription = m_node->lastError().description();
    //        for (auto variableValue : m_variableValues) {
    //            variableValue->value = errorDescription;
    //        }
    //    }

    for (auto variableValue : m_variableValues) {
        variableValue->value = variableValue->variable->readValue();
    }

    OperatorClickReport::instance()->write(QStringLiteral("Нажата кнопка \"Опросить\" в режиме ручного тестирования (протокол Modbus)"));
}

void ModbusLinkTedtingWidget::clear()
{
    for (auto variableValue : m_variableValues) {
        variableValue->value = QVariant();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Нажата кнопка \"Очистить\" в режиме ручного тестирования (протокол Modbus)"));
}

void ModbusLinkTedtingWidget::testKsuConnection()
{
    int errorCount = 0;
    int requestCount = ui->requestCountSpinBox->value();
    int pauseTime = ui->pauseSpinBox->value();

    ui->errorsLabel->setText(QStringLiteral("0 %"));
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(requestCount);
    ui->progressBar->setValue(0);

    m_txCount = 0;
    m_rxCount = 0;
    m_errorCount = 0;
    for (int i = 0; i < requestCount; i++, m_txCount++) {
        if (!m_testing) {
            break;
        }

        auto start = std::chrono::high_resolution_clock::now();

        // ??????????????????????????????????
        //        if (!m_node->readKsu0x10(INode::OperationTimingType::Sync)) {
        //            errorCount++;
        //            m_errorCount++;
        //        }
        //        else
        //            m_rxCount++;

        QApplication::processEvents(QEventLoop::AllEvents, pauseTime);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        auto msec = pauseTime - elapsed;
        if (msec > 0) {
            QThread::msleep(msec);
        }

        ui->progressBar->setValue(i);

        int errorCountPercentage = errorCount * 100 / double(requestCount);
        ui->errorsLabel->setText(QString::number(errorCountPercentage) + " %");
    }

    if (m_testing) {
        test();
    }
}
