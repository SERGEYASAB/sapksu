#include "Ksu735Manual0x10TestingWidget.h"
#include "ui_Ksu735Manual0x10TestingWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsVariable/Ksu735Variable.h"
#include "DBConfig/DBConfig.h"
#include "KSUPortConnection/KSUPortConnectionWidget.h"
#include "Authentication/SystemUser.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "ReportManager/OperatorClickReport.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

Ksu735Manual0x10TestingWidget::Ksu735Manual0x10TestingWidget(Ksu735Node *node, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ksu735Manual0x10TestingWidget),
    m_node(node)
{
    ui->setupUi(this);

    connect(ui->testButton, &QPushButton::clicked, this, &Ksu735Manual0x10TestingWidget::test);
    connect(ui->requestButton, &QPushButton::clicked, this, &Ksu735Manual0x10TestingWidget::request);
    connect(ui->clearButton, &QPushButton::clicked, this, &Ksu735Manual0x10TestingWidget::clear);

    ui->progressBar->hide();
    ui->progressBar->setContentsMargins(QMargins());

    ui->progressLine->resize(24, 0);
    ui->progressLine->setContentsMargins(QMargins());
    ui->progressLine->setMinimumHeight(24);

    if (m_node) {
        ui->addressLabel->setText(QString::number(node->address()));

        VariableValueList variableValueList;

        for (auto variable : m_node->ksu0x10Variables()) {
            VariableValue *variableValue = new VariableValue();
            variableValue->variable = variable;
            variableValue->value = QVariant();

            m_variableValues.append(variableValue);

            //            modelVariables.append(QPair<Ksu735Variable *, QVariant &>(variable, iter.value()));
        }

        m_model = new Ksu735Manual0x10TestingModel(m_variableValues, this);

        SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
        searchWidget->setSourceModel(m_model);
        searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
        ui->topLayout->addWidget(searchWidget);

        ui->tableView->setModel(searchWidget->getProxyModel());
        ui->tableView->setSelectionBehavior(QTableView::SelectRows);
        ui->tableView->setSelectionMode(QTableView::SelectionMode::SingleSelection);
        ui->tableView->setFocusPolicy(Qt::FocusPolicy::NoFocus);

        QHeaderView *verticalHeader = ui->tableView->verticalHeader();
        verticalHeader->setDefaultSectionSize(22);
        verticalHeader->hide();

        QHeaderView *horizontalHeader = ui->tableView->horizontalHeader();
        horizontalHeader->resizeSection(0, 150);
        horizontalHeader->resizeSection(1, 400);
        horizontalHeader->setStretchLastSection(true);
    } else {
        setEnabled(false);
    }
}

Ksu735Manual0x10TestingWidget::~Ksu735Manual0x10TestingWidget()
{
    delete ui;

    for (auto variableValue : m_variableValues) {
        delete variableValue;
    }
    m_variableValues.clear();
}

void Ksu735Manual0x10TestingWidget::stopTesting()
{
    if (m_testing)
        test();
}

void Ksu735Manual0x10TestingWidget::test()
{
    QWidgetList widgets;
    widgets << ui->requestCountSpinBox << ui->pauseSpinBox
            << ui->requestButton << ui->clearButton;

    m_testing = !m_testing;
    for (auto widget : widgets)
        widget->setEnabled(!m_testing);

    ui->progressBar->setVisible(m_testing);

    if (m_testing) {
        OperatorClickReport::instance()->write(QStringLiteral("Начато ручное тестирование КСУ по функции 0х10 (протокол 7.35)"));
        ui->testButton->setText("Стоп");
        m_testBeginDt.setDate(QDate::currentDate());
        m_testBeginDt.setTime(QTime::currentTime());
        testKsuConnection();
    } else {
        ui->testButton->setText("Тест");
        OperatorClickReport::instance()->write(QStringLiteral("Завершено ручное тестирование КСУ по функции 0х10 (протокол 7.35)"));
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

void Ksu735Manual0x10TestingWidget::request()
{
    if (m_node->readKsu0x10(INode::OperationTimingType::Sync)) {
        for (auto variableValue : m_variableValues) {
            bool use = variableValue->variable->useMinMax();
            variableValue->variable->setUseMinMax(false);
            variableValue->value = variableValue->variable->descriptionValue();
            variableValue->variable->setUseMinMax(use);
        }
    } else {
        QVariant errorDescription = m_node->lastError().description();
        for (auto variableValue : m_variableValues) {
            bool use = variableValue->variable->useMinMax();
            variableValue->variable->setUseMinMax(false);
            variableValue->value = errorDescription;
            variableValue->variable->setUseMinMax(use);
        }
    }
    OperatorClickReport::instance()->write(QStringLiteral("Нажата кнопка \"Опросить\" в режиме ручного тестирования (протокол 7.35)"));
    m_model->updateValues();
}

void Ksu735Manual0x10TestingWidget::clear()
{
    for (auto variableValue : m_variableValues) {
        variableValue->value = QVariant();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Нажата кнопка \"Очистить\" в режиме ручного тестирования (протокол 7.35)"));
    m_model->updateValues();
}

//void Ksu735Manual0x10TestingWidget::startKsu()
//{

//}

//void Ksu735Manual0x10TestingWidget::stopKsu()
//{

//}

//void Ksu735Manual0x10TestingWidget::deblocKsu()
//{

//}

//void Ksu735Manual0x10TestingWidget::ksuTime()
//{

//}

void Ksu735Manual0x10TestingWidget::testKsuConnection()
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

        if (!m_node->readKsu0x10(INode::OperationTimingType::Sync)) {
            errorCount++;
            m_errorCount++;
        }
        else
            m_rxCount++;

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
