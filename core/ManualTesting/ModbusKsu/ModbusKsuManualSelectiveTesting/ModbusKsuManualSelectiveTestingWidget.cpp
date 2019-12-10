#include "ModbusKsuManualSelectiveTestingWidget.h"
#include "ui_ModbusKsuManualSelectiveTestingWidget.h"

#include "ModbusKsuManualSelectiveTestingModel.h"
#include "ModbusKsuManualSelectiveTestingVariableDelegate.h"

#include "IOServer/nsNode/ModbusKsuNode.h"
#include "IOServer/nsVariable/ModbusVariable.h"

#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "ModbusKsuManualSelectiveTestingVariableDelegate.h"

#include "DBConfig/DBConfig.h"
#include "ReportManager/OperatorClickReport.h"
#include "ReportManager/FaultReport.h"

#include "ManualTesting/ModbusKsu/ModbusKsuManualTestingValueDelegate.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

#include "ErrorClassifier/ErrorClassifier.h"

static QString ModbusKsuPresetContainerName = "KsuModbusParametrsPreset";
static QString ModbusKsuPresetParamertTag = "KsuModbusParametr";

static const int ParametrsCount = 100;

ModbusKsuManualSelectiveTestingWidget::ModbusKsuManualSelectiveTestingWidget(ModbusKsuNode *node, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModbusKsuManualSelectiveTestingWidget),
    m_node(node)
{
    ui->setupUi(this);

    connect(ui->requestReadButton, &QPushButton::clicked, this, &ModbusKsuManualSelectiveTestingWidget::requestReadVariables);
    connect(ui->requestWriteButton, &QPushButton::clicked, this, &ModbusKsuManualSelectiveTestingWidget::requestWriteVariables);
    connect(ui->requestClearButton, &QPushButton::clicked, this, &ModbusKsuManualSelectiveTestingWidget::requestClear);

    connect(ui->loadSettingsButton, &QPushButton::clicked, this, &ModbusKsuManualSelectiveTestingWidget::loadSettings);
    connect(ui->saveSettingsButton, &QPushButton::clicked, this, &ModbusKsuManualSelectiveTestingWidget::saveSettings);
    connect(ui->clearSettingsButton, &QPushButton::clicked, this, &ModbusKsuManualSelectiveTestingWidget::clearSettings);

    for (int i = 0; i < ParametrsCount; i++) {
        m_parametrsList.append(new ModbusKsuManualSelectiveTestingParametr());
    }

    m_model = new ModbusKsuManualSelectiveTestingModel(node, m_parametrsList, this);

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    ui->horizontalLayout->addWidget(searchWidget);

    ui->tableView->setModel(searchWidget->getProxyModel());
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SelectionMode::SingleSelection);

    ModbusKsuManualSelectiveTestingVariableDelegate *variableDelegate = new ModbusKsuManualSelectiveTestingVariableDelegate(m_node, m_model, this);
    ui->tableView->setItemDelegateForColumn(1, variableDelegate);


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


    QHeaderView *verticalHeader = ui->tableView->verticalHeader();
    verticalHeader->setDefaultSectionSize(22);
    verticalHeader->hide();

    QHeaderView *horizontalHeader = ui->tableView->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(1, QHeaderView::Fixed);
    horizontalHeader->setStretchLastSection(true);

    horizontalHeader->resizeSections(QHeaderView::ResizeToContents);
    horizontalHeader->resizeSection(1, 70);
    horizontalHeader->resizeSection(2, 600);
}


ModbusKsuManualSelectiveTestingWidget::~ModbusKsuManualSelectiveTestingWidget()
{
    delete ui;
}

void ModbusKsuManualSelectiveTestingWidget::stopTesting()
{
    if (m_testing)
        startStopTest();
}

void ModbusKsuManualSelectiveTestingWidget::requestReadVariables()
{
    startStopTest();
    if (!m_testing)
        return;

    for (auto parametr : m_parametrsList) {
        if (!m_testing) {
            break;
        }

        if (!parametr->enabled || !parametr->variable)
            continue;

        bool use = parametr->variable->useMinMax();
        parametr->variable->setUseMinMax(false);

        if (m_node->readVariable(parametr->variable, INode::OperationTimingType::Sync)) {
            parametr->readedValue = parametr->variable->descriptionValue();
            parametr->variable->setUseMinMax(use);
        } else {
            parametr->readedValue = m_node->lastError().description();
            parametr->variable->setUseMinMax(use);
        }

        QApplication::processEvents(QEventLoop::AllEvents, 100);
        m_model->updateParametr(parametr);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Считать переменную\""));
}

void ModbusKsuManualSelectiveTestingWidget::requestWriteVariables()
{
    startStopTest();
    if (!m_testing)
        return;

    for (auto parametr : m_parametrsList) {
        if (!m_testing) {
            break;
        }

        if (!parametr->enabled || !parametr->variable)
            continue;

        if (parametr->writedValue.isValid()) {
            if (m_node->writeVariable(parametr->variable, parametr->writedValue, INode::OperationTimingType::Sync)) {

            }
        }

        if (m_node->readVariable(parametr->variable, INode::OperationTimingType::Sync)) {
            parametr->readedValue = parametr->variable->descriptionValue();
        } else {
            parametr->readedValue = m_node->lastError().description();
        }

        QApplication::processEvents(QEventLoop::AllEvents, 100);
        m_model->updateParametr(parametr);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать переменную\""));
}

void ModbusKsuManualSelectiveTestingWidget::requestClear()
{
    for (auto parametr : m_parametrsList) {
        if (!parametr)
            continue;

        parametr->writedValue = QVariant();
        parametr->readedValue = QVariant();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Очистить\""));
    m_model->updateValues();
}

void ModbusKsuManualSelectiveTestingWidget::loadSettings()
{
    QString loadFilePath = QFileDialog::getOpenFileName(this, QStringLiteral("Открыть"),
                                                        DBConfig::ksuManualSelectiveTestingPresetsPath(),
                                                        QStringLiteral("*.cfg"));

    if (loadFilePath.isEmpty())
        return;

    QFile configFile(loadFilePath);
    if (!configFile.exists(loadFilePath)) {
        ErrorClassifier::instance()->showError(46);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(46));
        return;
    }

    if (!configFile.open(QFile::ReadOnly)) {
        ErrorClassifier::instance()->showError(46);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(46));
        return;
    }

    QDomDocument document;
    if (!document.setContent(&configFile)) {
        ErrorClassifier::instance()->showError(47);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(47));
        return;
    }
    configFile.close();

    QDomElement containerElement = document.firstChildElement(ModbusKsuPresetContainerName);
    if (containerElement.isNull()) {
        ErrorClassifier::instance()->showError(47);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(47));
        return;
    }

    clearSettings();

    QDomNodeList parametrsNodes = containerElement.childNodes();
    int count = parametrsNodes.count();
    for (int i = 0; i < qMin(count, ParametrsCount); i++) {
        QDomNode parametrNode = parametrsNodes.at(i);
        if (parametrNode.isNull() || !parametrNode.isElement()) continue;

        QDomElement parametrElement = parametrNode.toElement();
        if (parametrElement.tagName() != ModbusKsuPresetParamertTag) continue;

        ModbusKsuManualSelectiveTestingParametr *parametr = m_parametrsList[i];

        bool ok;
        QString addressAttribute = parametrElement.attribute(QStringLiteral("address"));
        quint32 address = addressAttribute.toUInt(&ok);
        ModbusVariable *variable = m_node->variableByAddress(address);

        parametr->address = address;

        if (addressAttribute.isNull() || !ok || !variable) continue;

        parametr->variable = variable;

        QString enabledAttribute = parametrElement.attribute(QStringLiteral("enabled"));
        int enabled = enabledAttribute.toInt(&ok);
        if (!enabledAttribute.isNull() && ok && enabled)
            parametr->enabled = true;

        QString writedValueAttribute = parametrElement.attribute(QStringLiteral("writedValue"));
        if (writedValueAttribute.isEmpty()) continue;

        QVariant writedValue;

        double doubleValue = writedValueAttribute.toDouble(&ok);
        if (ok)
            writedValue.setValue(doubleValue);
        else {
            int intValue = writedValueAttribute.toInt(&ok);
            if (ok)
                writedValue.setValue(intValue);
            else
                writedValue.setValue(writedValueAttribute);
        }

        parametr->writedValue = writedValue;
    }

    m_model->updateValues();
    OperatorClickReport::instance()->write(QString("Ручное тестирование: загрузка конфигурации \"%1\"").arg(loadFilePath));
}

void ModbusKsuManualSelectiveTestingWidget::saveSettings()
{
    QDir dir(DBConfig::ksuManualSelectiveTestingPresetsPath());
    dir.mkpath(dir.path());

    QString saveFilePath = QFileDialog::getSaveFileName(this,
                                                        QStringLiteral("Сохранить как"),
                                                        DBConfig::ksuManualSelectiveTestingPresetsPath(),
                                                        QStringLiteral("*.cfg"));

    if (saveFilePath.isEmpty())
        return;

    QDomDocument document;
    QDomProcessingInstruction instructions = document.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
    document.appendChild(instructions);

    QDomElement parametrsContainerElement = document.createElement(ModbusKsuPresetContainerName);
    document.appendChild(parametrsContainerElement);

    for (auto parametr : m_parametrsList) {
        if (!parametr) continue;

        if (parametr->address != -1) {
            QDomElement parametrElement = document.createElement(ModbusKsuPresetParamertTag);

            if (parametr->variable) {
                parametrElement.setAttribute(QStringLiteral("address"), parametr->variable->address());
            } else {
                parametrElement.setAttribute(QStringLiteral("address"), parametr->address);
            }
            parametrElement.setAttribute(QStringLiteral("enabled"), parametr->enabled);
            parametrElement.setAttribute(QStringLiteral("writedValue"), parametr->writedValue.toString());
            parametrElement.setAttribute(QStringLiteral("type"), parametr->writedValue.typeName());

            parametrsContainerElement.appendChild(parametrElement);
        }
    }

    QFile configFile(saveFilePath);
    configFile.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream stream(&configFile);
    document.save(stream, 4);
    configFile.close();
    OperatorClickReport::instance()->write(QString("Ручное тестирование: сохранение конфигурации \"%1\"").arg(saveFilePath));
}

void ModbusKsuManualSelectiveTestingWidget::clearSettings()
{
    for (auto parametr : m_parametrsList) {
        if (!parametr)
            continue;

        parametr->address = -1;
        parametr->enabled = false;
        parametr->variable = nullptr;
        parametr->writedValue = QVariant();
        parametr->readedValue = QVariant();
    }

    m_model->updateValues();
}

void ModbusKsuManualSelectiveTestingWidget::startStopTest()
{
    QWidgetList widgets;
    widgets << ui->requestWriteButton << ui->requestClearButton
            << ui->loadSettingsButton << ui->saveSettingsButton
            << ui->clearSettingsButton;

    m_testing = !m_testing;
    for (auto widget : widgets)
        widget->setEnabled(!m_testing);

//    ui->progressBar->setVisible(m_testing);

    if (m_testing) {
        ui->requestReadButton->setText("Стоп");
    } else {
        ui->requestReadButton->setText("Чтение");
    }
}
