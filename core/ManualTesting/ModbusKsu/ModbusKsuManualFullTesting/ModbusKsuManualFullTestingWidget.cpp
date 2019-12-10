#include "ModbusKsuManualFullTestingWidget.h"
#include "ui_ModbusKsuManualFullTestingWidget.h"

#include "IOServer/nsNode/ModbusKsuNode.h"
#include "IOServer/nsVariable/ModbusVariable.h"

#include "ModbusKsuManualFullTestingModel.h"
#include "ManualTesting/ModbusKsu/ModbusKsuManualTestingValueDelegate.h"

#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "AppSettings/AppSettings.h"
#include "ReportManager/OperatorClickReport.h"

#include "Common/QMessageBox/ScriptDelayMessageBox/ProgressBarMessageBox.h"

#include "Authentication/SystemUser.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "IOServer/IOServer.h"
#include "DBConfig/DBConfig.h"
#include "ManualTesting/ModbusKsu/ModbusKsuManualTestingParametr.h"

#include "KSUPortConnection/KSUPortConnectionWidget.h"

#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

static const int RequestsSleepTime = 100;

static const QString okColorKey         = "ModbusKsuManualFullTestingWidget_OkColor";
static const QString notSameColorKey    = "ModbusKsuManualFullTestingWidget_NotSameColor";
static const QString errorColorKey      = "ModbusKsuManualFullTestingWidget_ErrorColor";

ModbusKsuManualFullTestingWidget::ModbusKsuManualFullTestingWidget(ModbusKsuNode *node, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModbusKsuManualFullTestingWidget),
    m_node(node)
{
    ui->setupUi(this);
    ui->loadSettingsButton->hide();
    ui->saveSettingsButton->hide();

    connect(ui->showAllCheckBox, &QCheckBox::stateChanged,
            this, &ModbusKsuManualFullTestingWidget::showAllCheckBoxChanged);
    connect(ui->enableAllCheckBox, &QCheckBox::stateChanged,
            this, &ModbusKsuManualFullTestingWidget::enableAllCheckBoxChanged);
    connect(ui->enableDefaultButton, &QPushButton::clicked,
            this, &ModbusKsuManualFullTestingWidget::enableDefault);

    connect(ui->writeDefaultButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::writeDefault);
    connect(ui->readButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::readAll);
    connect(ui->autotestButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::autotest);
    connect(ui->clearButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::clear);

    connect(ui->writeMinimumButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::writeAllMinimums);
    connect(ui->writeMaximumButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::writeAllMaximums);

    connect(ui->writeButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::writeAll);
    connect(ui->clearSettingsButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::clearWriting);

    connect(ui->okColorButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::okColorButton);
    connect(ui->notSameColorButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::notSameColorButton);
    connect(ui->errorColorButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::errorColorButton);

    connect(ui->testLinkButton, &QPushButton::clicked, this, &ModbusKsuManualFullTestingWidget::testLink);

    ui->addressLabel->setText(QString::number(node->address()));

    for (auto variable : m_node->commonVariables()) {
        ModbusKsuManualTestingParametr *parametr = new ModbusKsuManualTestingParametr(variable);
        //        parametr->enabled = variable->isEnabled();
        m_parametrsList.append(parametr);
    }

    m_model = new ModbusKsuManualFullTestingModel(node, m_parametrsList, this);

    auto appSettings = AppSettings::settings();
    QColor okColor = QColor(appSettings->value(okColorKey, QColor(Qt::green).name(QColor::HexRgb)).toString());
    QColor notSameColor = QColor(appSettings->value(notSameColorKey, QColor(Qt::yellow).name(QColor::HexRgb)).toString());
    QColor errorColor = QColor(appSettings->value(errorColorKey, QColor(Qt::red).name(QColor::HexRgb)).toString());

    m_model->setOkColor(okColor);
    ui->okColorButton->setStyleSheet("background-color: " + okColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
    m_model->setNotSameColor(notSameColor);
    ui->notSameColorButton->setStyleSheet("background-color: " + notSameColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
    m_model->setErrorColor(errorColor);
    ui->errorColorButton->setStyleSheet("background-color: " + errorColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    ui->searchFormLayout->addWidget(searchWidget);

    ui->tableView->setModel(searchWidget->getProxyModel());
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SelectionMode::SingleSelection);
    //    ui->tableView->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    //    ui->tableView->setItemDelegateForColumn(1, new Ksu735Manual0x20TestingVariableDelegate(node, m_model, this));


    QMap<int, QString> deviceDataTypeDelegateMap;
    QMetaEnum deviceDataTypesMetaEnum = QMetaEnum::fromType<ModbusVariable::ModbusDataType>();
    for (int i = 0; i < deviceDataTypesMetaEnum.keyCount(); i++) {
        deviceDataTypeDelegateMap.insert(deviceDataTypesMetaEnum.value(i), deviceDataTypesMetaEnum.key(i));
    }
    FKComboBoxDelegate *deviceDataTypeDelegate = new FKComboBoxDelegate(deviceDataTypeDelegateMap, this);
    ui->tableView->setItemDelegateForColumn(4, deviceDataTypeDelegate);


    QMap<int, QString> serverDataTypeDelegateMap;
    QMetaEnum serverDataTypesMetaEnum = QMetaEnum::fromType<IVariable::DataType>();
    for (int i = 0; i < serverDataTypesMetaEnum.keyCount(); i++) {
        serverDataTypeDelegateMap.insert(serverDataTypesMetaEnum.value(i), serverDataTypesMetaEnum.key(i));
    }
    FKComboBoxDelegate *serverDataTypeDelegate = new FKComboBoxDelegate(serverDataTypeDelegateMap, this);
    ui->tableView->setItemDelegateForColumn(5, serverDataTypeDelegate);


    QMap<int, QString> keyValueMap;
    keyValueMap[0] = "R";
    keyValueMap[1] = "R/W";
    FKComboBoxDelegate *writeAccessDelegate = new FKComboBoxDelegate(keyValueMap, this);
    ui->tableView->setItemDelegateForColumn(6, writeAccessDelegate);

    ModbusKsuManualTestingValueDelegate *valueDelegate = new ModbusKsuManualTestingValueDelegate(m_model, this);
    ui->tableView->setItemDelegateForColumn(11, valueDelegate);
    ui->tableView->setItemDelegateForColumn(12, valueDelegate);
    ui->tableView->setItemDelegateForColumn(13, valueDelegate);
    ui->tableView->setItemDelegateForColumn(14, valueDelegate);

    QHeaderView *verticalHeader = ui->tableView->verticalHeader();
    verticalHeader->setDefaultSectionSize(22);
    verticalHeader->hide();

#ifdef SAP_KSU
    QHeaderView *horizontalHeader = ui->tableView->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(1, QHeaderView::Fixed);
    horizontalHeader->setStretchLastSection(true);

    horizontalHeader->resizeSections(QHeaderView::ResizeToContents);
    horizontalHeader->resizeSection(1, 70);
    horizontalHeader->resizeSection(2, 500);
    horizontalHeader->resizeSection(7, 50);
    horizontalHeader->resizeSection(8, 50);
    horizontalHeader->resizeSection(12, 50);
    horizontalHeader->resizeSection(13, 50);
#endif
#ifdef SAP_SU
    QHeaderView *horizontalHeader = ui->tableView->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(1, QHeaderView::Fixed);
    horizontalHeader->setStretchLastSection(true);

    horizontalHeader->resizeSections(QHeaderView::ResizeToContents);
    horizontalHeader->resizeSection(1, 70);
    horizontalHeader->resizeSection(2, 500);
    horizontalHeader->resizeSection(7, 50);
    horizontalHeader->resizeSection(8, 50);
    horizontalHeader->resizeSection(12, 50);
    horizontalHeader->resizeSection(13, 50);
#endif
#ifdef MECHOS
    QHeaderView *horizontalHeader = ui->tableView->horizontalHeader();

    for (int a = 0; a < horizontalHeader->count(); a++) {
        horizontalHeader->setSectionResizeMode(a, QHeaderView::ResizeToContents);
    }

    horizontalHeader->setSectionHidden(4, true);
    horizontalHeader->setSectionHidden(5, true);
    horizontalHeader->setSectionHidden(10, true);
    horizontalHeader->setStretchLastSection(true);

#endif

    ui->progressBar->hide();
    ui->progressBar->setContentsMargins(QMargins());

    ui->progressLine->resize(24, 0);
    ui->progressLine->setContentsMargins(QMargins());
    ui->progressLine->setMinimumHeight(24);

    m_progressBarMessageBox = new ProgressBarMessageBox(this);

    readSettings();
}

ModbusKsuManualFullTestingWidget::~ModbusKsuManualFullTestingWidget()
{
    delete ui;

    for (auto parametr : m_parametrsList) {
        delete parametr;
    }
    m_parametrsList.clear();
}

void ModbusKsuManualFullTestingWidget::stopTesting()
{
    if (m_testing)
        startStopTest();
}

void ModbusKsuManualFullTestingWidget::showAllCheckBoxChanged(int state)
{
    if (state == Qt::CheckState::Unchecked) {
        m_model->setShowEnabledOnly(true);
    } else {
        m_model->setShowEnabledOnly(false);
    }
}

void ModbusKsuManualFullTestingWidget::enableAllCheckBoxChanged(int state)
{
    for (auto parametr : m_parametrsList) {
        parametr->enabled = state == Qt::CheckState::Checked;
    }

    m_model->updateEnabledParametrsList();
}

void ModbusKsuManualFullTestingWidget::enableDefault()
{
    for (auto parametr : m_parametrsList) {
        parametr->enabled = parametr->variable()->isEnabled();
    }

    const QSignalBlocker blocker(ui->enableAllCheckBox);
    ui->enableAllCheckBox->setChecked(false);

    m_model->updateEnabledParametrsList();
}

void ModbusKsuManualFullTestingWidget::writeDefault()
{
    startStopTest();
    if (!m_testing)
        return;

    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    QList<ModbusKsuManualTestingParametr *> enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->variable()->defaultValue());
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать значения по-умолчанию\""));
}

void ModbusKsuManualFullTestingWidget::readAll()
{
    startStopTest();
    if (!m_testing)
        return;

    // Debug
    //    for (int i = 0; i < m_parametrsList.count(); i++) {
    //        if (i > 3)
    //        m_parametrsList.at(i)->enabled = false;
    //    }

    QList<ModbusKsuManualTestingParametr *> enabledParametrsList = enabledParametrs();

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        readOneParametr(parametr);
        m_model->updateParametr(parametr);
        processEventsAndSleep(RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
        setProgressBarProgress(double(ui->progressBar->value()) / double(ui->progressBar->maximum()) * 100);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Считать все параметры\""));
}

void ModbusKsuManualFullTestingWidget::readSettings()
{
    auto settings = AppSettings::settings();
    for (int i = 0; i < ui->tableView->model()->columnCount(); i++)
        ui->tableView->setColumnWidth(i, settings->value(QString(metaObject()->className()) + QString("/column%1Width").arg(i), 100).toInt());

}

void ModbusKsuManualFullTestingWidget::testLink()
{
    startStopTest();
    if (!m_testing)
        return;

    QList<ModbusKsuManualTestingParametr *> enabledParametrsList = enabledParametrs();
    if (enabledParametrsList.isEmpty()) {
        startStopTest();
        return;
    }

    OperatorClickReport::instance()->write(QStringLiteral("Начато ручное тестирование КСУ (протокол Modbus)"));

    auto it = enabledParametrsList.cbegin();

    int errorCount = 0;
    int requestCount = ui->requestCountSpinBox->value();
    int pauseTime = ui->pauseSpinBox->value();

    ui->errorsLabel->setText(QStringLiteral("0 %"));
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(requestCount);
    ui->progressBar->setValue(0);

    m_testBeginDt.setDate(QDate::currentDate());
    m_testBeginDt.setTime(QTime::currentTime());

    m_txCount = 0;
    m_rxCount = 0;
    m_errorCount = 0;
    for (int i = 0; i < requestCount; i++, m_txCount++) {
        if (!m_testing) {
            break;
        }

        auto start = std::chrono::high_resolution_clock::now();

        if (!readOneParametr(*it)) {
            errorCount++;
            m_errorCount++;
        } else {
            m_rxCount++;
        }

        m_model->updateParametr(*it);
        processEventsAndSleep(RequestsSleepTime);

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        auto msec = pauseTime - elapsed;
        if (msec > 0) {
            QThread::msleep(msec);
        }

        ui->progressBar->setValue(i);

        int errorCountPercentage = errorCount * 100 / double(requestCount);
        ui->errorsLabel->setText(QString::number(errorCountPercentage) + " %");

        if (++it == enabledParametrsList.cend()) {
            it = enabledParametrsList.cbegin();
        }
    }

    if (m_testing) {
        startStopTest();
    }

    OperatorClickReport::instance()->write(QStringLiteral("Завершено ручное тестирование КСУ (протокол Modbus)"));
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

bool ModbusKsuManualFullTestingWidget::autotest()
{
    bool autotestresult = true;

    startStopTest();
    if (!m_testing)
        return false;

    if (autotestingmodeFlag)
        m_progressBarMessageBox->show();

    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    QList<ModbusKsuManualTestingParametr *> enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        if (!autotestParametr(parametr)) {
            autotestresult = false;
        }

        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
        setProgressBarProgress(double(ui->progressBar->value()) / double(ui->progressBar->maximum()) * 100);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Автотест\""));

    return autotestresult;
}

void ModbusKsuManualFullTestingWidget::clear()
{
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Очистить\""));
    clearAll();
}

void ModbusKsuManualFullTestingWidget::writeAllMinimums()
{
    startStopTest();
    if (!m_testing)
        return;

    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    QList<ModbusKsuManualTestingParametr *> enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->variable()->minValue());
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);

    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать все минимумы\""));
}

void ModbusKsuManualFullTestingWidget::writeAllMaximums()
{
    startStopTest();
    if (!m_testing)
        return;

    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    QList<ModbusKsuManualTestingParametr *> enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->variable()->maxValue());
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать все максимумы\""));
}

void ModbusKsuManualFullTestingWidget::writeAll()
{
    startStopTest();
    if (!m_testing)
        return;

    QList<ModbusKsuManualTestingParametr *> writedParametrsList;
    for (auto parametr : m_parametrsList) {
        if (parametr->enabled && parametr->writingValue.isValid() && !parametr->writingValue.isNull()) {
            writedParametrsList.append(parametr);
        }
    }

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(writedParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : writedParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->writingValue);
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать все параметры\""));
}

void ModbusKsuManualFullTestingWidget::clearWriting()
{
    for (auto parametr : m_parametrsList) {
        parametr->clearWritingValue();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Очистить хаписваемые параметры\""));
    m_model->updateColumn(10);
}

void ModbusKsuManualFullTestingWidget::okColorButton()
{
    QColorDialog colorDialog(m_model->okColor());
    colorDialog.exec();
    if (colorDialog.result() == QColorDialog::Accepted) {
        QColor selectedColor = colorDialog.selectedColor();
        ui->okColorButton->setStyleSheet("background-color: " + selectedColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
        m_model->setOkColor(selectedColor);
        AppSettings::settings()->setValue(okColorKey, selectedColor.name(QColor::HexRgb));
    }
}

void ModbusKsuManualFullTestingWidget::notSameColorButton()
{
    QColorDialog colorDialog(m_model->notSameColor());
    colorDialog.exec();
    if (colorDialog.result() == QColorDialog::Accepted) {
        QColor selectedColor = colorDialog.selectedColor();
        ui->notSameColorButton->setStyleSheet("background-color: " + selectedColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
        m_model->setNotSameColor(selectedColor);
        AppSettings::settings()->setValue(notSameColorKey, selectedColor.name(QColor::HexRgb));
    }
}

void ModbusKsuManualFullTestingWidget::errorColorButton()
{
    QColorDialog colorDialog(m_model->errorColor());
    colorDialog.exec();
    if (colorDialog.result() == QColorDialog::Accepted) {
        QColor selectedColor = colorDialog.selectedColor();
        ui->errorColorButton->setStyleSheet("background-color: " + selectedColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
        m_model->setErrorColor(selectedColor);
        AppSettings::settings()->setValue(errorColorKey, selectedColor.name(QColor::HexRgb));
    }
}

void ModbusKsuManualFullTestingWidget::startStopTest()
{
    QWidgetList widgets;
    widgets << ui->showAllCheckBox << ui->enableAllCheckBox
            << ui->ignoreReadOnlyCheckBox
            << ui->enableDefaultButton << ui->writeDefaultButton
            << ui->autotestButton << ui->clearButton

            << ui->writeMinimumButton << ui->writeMaximumButton

            << ui->writeButton << ui->clearSettingsButton
            << ui->testLinkButton << ui->requestCountSpinBox
            << ui->pauseSpinBox << ui->spQualitySpin;

    m_testing = !m_testing;
    for (auto widget : widgets)
        widget->setEnabled(!m_testing);

    ui->progressBar->setVisible(m_testing);

    if (m_testing) {
        clearAll();
        ui->readButton->setText("Стоп");
    } else {
        ui->readButton->setText("Чтение");
    }
}

bool ModbusKsuManualFullTestingWidget::readOneParametr(ModbusKsuManualTestingParametr *parametr)
{
    bool use = parametr->variable()->useMinMax();
    parametr->variable()->setUseMinMax(false);
    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->readedValue = parametr->variable()->descriptionValue();
        parametr->status = SapKsu::FullTestingStatus::Ok;
        parametr->variable()->setUseMinMax(use);
        return true;
    } else {
        parametr->readedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        parametr->variable()->setUseMinMax(use);
        return false;
    }
}

bool ModbusKsuManualFullTestingWidget::writeOneParametr(ModbusKsuManualTestingParametr *parametr, const QVariant &value)
{
    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->oldReadedValue = parametr->variable()->value();
    } else {
        parametr->oldReadedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    QVariant writingValue = value;
    if (!writingValue.isValid() || writingValue.isNull()) {
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = QStringLiteral("Неверное значение записываемого параметра");
    }

    if (m_node->writeVariable(parametr->variable(), writingValue, INode::OperationTimingType::Sync)) {
        parametr->writedValue = writingValue;
    } else {
        parametr->writedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->readedValue = parametr->variable()->value();
    } else {
        parametr->readedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    parametr->statusString.clear();
    if (compareReadedAndWritedValue(parametr->readedValue, parametr->writedValue, parametr->variable())) {
        parametr->status = SapKsu::FullTestingStatus::Ok;
    } else {
        parametr->status = SapKsu::FullTestingStatus::NotSame;
    }

    return true;
}

bool ModbusKsuManualFullTestingWidget::autotestParametr(ModbusKsuManualTestingParametr *parametr)
{
    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->oldReadedValue = parametr->variable()->value();
    } else {
        parametr->oldReadedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    QVariant writingValue = incrementValue(parametr->oldReadedValue, parametr->variable());

    if (!writingValue.isValid() || writingValue.isNull()) {
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = QStringLiteral("Неверное значение записываемого параметра");
    }

    if (m_node->writeVariable(parametr->variable(), writingValue, INode::OperationTimingType::Sync)) {
        parametr->writedValue = writingValue;
    } else {
        parametr->writedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->readedValue = parametr->variable()->value();
    } else {
        parametr->readedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    parametr->statusString.clear();
    if (compareReadedAndWritedValue(parametr->readedValue, parametr->writedValue, parametr->variable())) {
        parametr->status = SapKsu::FullTestingStatus::Ok;

        if (!m_node->writeVariable(parametr->variable(), parametr->oldReadedValue, INode::OperationTimingType::Sync)) {
            parametr->status = SapKsu::FullTestingStatus::Error;
            parametr->statusString = QStringLiteral("Не удалось записать исходное значение");
            return false;
        }

        if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
            QVariant readedValue = parametr->variable()->value();

            if (!compareReadedAndWritedValue(readedValue, parametr->oldReadedValue, parametr->variable())) {
                parametr->status = SapKsu::FullTestingStatus::Error;
                parametr->statusString = QStringLiteral("Не удалось записать исходное значение");
                return false;
            }

        } else {
            parametr->status = SapKsu::FullTestingStatus::Error;
            parametr->statusString = QStringLiteral("Не удалось записать исходное значение");
            return false;
        }


    } else {
        parametr->status = SapKsu::FullTestingStatus::NotSame;
    }

    return true;
}

QVariant ModbusKsuManualFullTestingWidget::incrementValue(const QVariant &value, ModbusVariable *variable)
{
    if (variable->serverValueDataType() == IVariable::DataType::Boolean) {
        bool boolValue = value.toBool();
        bool newValue = !boolValue;
        return QVariant::fromValue(newValue);
    } else if (variable->serverValueDataType() == IVariable::DataType::Float32 || variable->serverValueDataType() == IVariable::DataType::Float64) {
        double doubleValue = value.toDouble();
        doubleValue += 1.0;
        if (doubleValue > variable->maxValue().toDouble()) {
            return variable->minValue();
        }
        return QVariant::fromValue(doubleValue);
    } else if (variable->serverValueDataType() == IVariable::DataType::DateTime) {
        QDateTime dateTimeValue = value.toDateTime();

        return dateTimeValue.addDays(1).addMonths(1).addYears(1).addSecs(3600 + 60 + 1);
    } else if (variable->serverValueDataType() == IVariable::String) {
        QString stringValue = value.toString();

        QTextCodec *codec = QTextCodec::codecForName("cp866");

        auto encoder = codec->makeEncoder();
        QByteArray bytes = encoder->fromUnicode(stringValue);

        if (bytes == QByteArray(bytes.size(), 0xFF)) {
            bytes = QByteArray(bytes.size(), 0x00);
        } else {
            for (qint8 pos = bytes.size() - 1; pos >= 0; pos--) {
                quint8 byte = quint8(bytes.at(pos));
                if (byte != 0xFF) {
                    byte++;
                    bytes[pos] = byte;
                    break;
                }
            }
        }
        auto decoder = codec->makeDecoder();
        QString newValue = decoder->toUnicode(bytes);
        return QVariant::fromValue(newValue);

    } else if (variable->serverValueDataTypeIsSigned()) {
        quint64 uintValue = value.toULongLong();
        uintValue++;
        if (uintValue > variable->maxValue().toUInt()) {
            return variable->minValue();
        }
        return QVariant::fromValue(uintValue);
    } else {
        qint64 intValue = value.toUInt();
        intValue++;
        if (intValue > variable->maxValue().toLongLong()) {
            return variable->minValue();
        }
        return QVariant::fromValue(intValue);
    }
}

void ModbusKsuManualFullTestingWidget::clearAll()
{
    for (auto parametr : m_parametrsList) {
        parametr->clear();
    }
    m_model->updateAll();
}

QList<ModbusKsuManualTestingParametr *> ModbusKsuManualFullTestingWidget::enabledParametrs(bool writeOnly)
{
    QList<ModbusKsuManualTestingParametr *> list;
    //list.append(m_parametrsList.first()); return list;
    for (auto parametr : m_parametrsList) {
        if (parametr->enabled) {
            if (!writeOnly || (writeOnly && parametr->variable()->isWritable()))
                list.append(parametr);
        }
    }
    return list;
}

void ModbusKsuManualFullTestingWidget::processEventsAndSleep(int sleepTime) const
{
    QApplication::processEvents(QEventLoop::AllEvents, sleepTime);
}

bool ModbusKsuManualFullTestingWidget::compareReadedAndWritedValue(const QVariant &rval,
                                                                   const QVariant &lval,
                                                                   ModbusVariable *variable)
{
    if (rval == lval) {
        return true;
    } else {
        QMetaType::Type type = static_cast<QMetaType::Type>(rval.type());
        if ((type == QMetaType::Double || type == QMetaType::Float) && lval.canConvert(type)) {
            double rDoubleValue = rval.value<double>();
            double lDoubleValue = lval.value<double>();

            if (variable->precision() != -1) {
                qint64 rRoundedValue = round(rDoubleValue * pow(10, variable->precision()));
                qint64 lRoundedValue = round(lDoubleValue * pow(10, variable->precision()));
                if (rRoundedValue == lRoundedValue)
                    return true;
            } else {
                return rDoubleValue == lDoubleValue;
            }
        }
    }

    return false;
}

void ModbusKsuManualFullTestingWidget::closeEvent(QCloseEvent *event)
{
    auto settings = AppSettings::settings();
    settings->beginGroup(metaObject()->className());
    for (int i = 0; i < ui->tableView->model()->columnCount(); i++)
        settings->setValue(QString("column%1Width").arg(i), ui->tableView->columnWidth(i));
    settings->endGroup();
    settings->sync();

    QWidget::closeEvent(event);
}

ProgressBarMessageBox *ModbusKsuManualFullTestingWidget::getProgressBarMessageBox() const
{
    return m_progressBarMessageBox;
}

int ModbusKsuManualFullTestingWidget::getProgressBarProgress() const
{
    return progressBarProgress;
}

void ModbusKsuManualFullTestingWidget::setProgressBarProgress(int value)
{
    if (progressBarProgress == value)
        return;

    progressBarProgress = value;

    emit progressBarChanged(value);
}

bool ModbusKsuManualFullTestingWidget::getAutotestingmodeFlag() const
{
    return autotestingmodeFlag;
}

void ModbusKsuManualFullTestingWidget::setAutotestingmodeFlag(bool value)
{
    autotestingmodeFlag = value;
}
