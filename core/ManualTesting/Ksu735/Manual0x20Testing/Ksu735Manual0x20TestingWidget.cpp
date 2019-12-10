#include "Ksu735Manual0x20TestingWidget.h"
#include "ui_Ksu735Manual0x20TestingWidget.h"

#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

#include "Ksu735Manual0x20TestingModel.h"
#include "Ksu735Manual0x20TestingVariableDelegate.h"
#include "Ksu735Manual0x20TestingValueDelegate.h"

#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"

#include "DBConfig/DBConfig.h"
#include "ReportManager/OperatorClickReport.h"
#include "ReportManager/FaultReport.h"

#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"
#include "ErrorClassifier/ErrorClassifier.h"

static QString KsuPresetContainerName = "KsuParametrsPreset";
static QString KsuPresetParamertTag = "KsuParametr";

Ksu735Manual0x20TestingWidget::Ksu735Manual0x20TestingWidget(Ksu735Node *node, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ksu735Manual0x20TestingWidget),
    m_node(node)
{   
    ui->setupUi(this);

    connect(ui->request0x20Button, &QPushButton::clicked, this, &Ksu735Manual0x20TestingWidget::request0x20);
    connect(ui->request0x40Button, &QPushButton::clicked, this, &Ksu735Manual0x20TestingWidget::request0x40);
    connect(ui->requestClearButton, &QPushButton::clicked, this, &Ksu735Manual0x20TestingWidget::requestClear);

    connect(ui->loadSettingsButton, &QPushButton::clicked, this, &Ksu735Manual0x20TestingWidget::loadSettings);
    connect(ui->saveSettingsButton, &QPushButton::clicked, this, &Ksu735Manual0x20TestingWidget::saveSettings);
    connect(ui->clearSettingsButton, &QPushButton::clicked, this, &Ksu735Manual0x20TestingWidget::clearSettings);

    for (int i = 0; i < 10; i++) {
        m_parametrsList.append(new Ksu735Manual0x20TestingParametr());
    }

    m_model = new Ksu735Manual0x20TestingModel(node, m_parametrsList, this);

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    ui->horizontalLayout->addWidget(searchWidget);

    ui->tableView->setModel(searchWidget->getProxyModel());
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SelectionMode::SingleSelection);
    //    ui->tableView->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    ui->tableView->setItemDelegateForColumn(1, new Ksu735Manual0x20TestingVariableDelegate(node, m_model, this));

    QMap<int, QString> delegateMap;
    delegateMap[0] = "XXXX";
    delegateMap[1] = "XXX.X";
    delegateMap[2] = "XX.XX";
    delegateMap[3] = "X.XXX";
    delegateMap[4] = ".XXXX";
    delegateMap[5] = "XX/XX";
    delegateMap[6] = "BIN";
    delegateMap[7] = "CCCC";
    FKComboBoxDelegate *formatDelegate = new FKComboBoxDelegate(delegateMap, this);
    ui->tableView->setItemDelegateForColumn(3, formatDelegate);

    QMap<int, QString> keyValueMap;
    keyValueMap[0] = "R";
    keyValueMap[1] = "R/W";
    FKComboBoxDelegate *writeAccessDelegate = new FKComboBoxDelegate(keyValueMap, this);
    ui->tableView->setItemDelegateForColumn(4, writeAccessDelegate);

    Ksu735Manual0x20TestingValueDelegate *valueDelegate = new Ksu735Manual0x20TestingValueDelegate(m_model, this);
    ui->tableView->setItemDelegateForColumn(9, valueDelegate);
    ui->tableView->setItemDelegateForColumn(10, valueDelegate);

    ui->tableView->setItemDelegateForColumn(5, valueDelegate);
    ui->tableView->setItemDelegateForColumn(6, valueDelegate);
    ui->tableView->setItemDelegateForColumn(7, valueDelegate);

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

Ksu735Manual0x20TestingWidget::~Ksu735Manual0x20TestingWidget()
{
    delete ui;

    for (auto parametr : m_parametrsList) {
        delete parametr;
    }
    m_parametrsList.clear();
}

void Ksu735Manual0x20TestingWidget::request0x20()
{
    Ksu735Manual0x20TestingParametrsList readedParametrs;
    QList<IVariable *> readedVariables;
    for (auto parametr : m_parametrsList) {
        if (!parametr) continue;

        if (parametr->variable && parametr->enabled) {
            readedVariables.append(parametr->variable);
            readedParametrs.append(parametr);
        }
    }

    if (!readedVariables.isEmpty()) {
        if (m_node->readVariables(readedVariables, Ksu735Node::OperationTimingType::Sync)) {
            for (auto parametr : readedParametrs) {
                bool use = parametr->variable->useMinMax();
                parametr->variable->setUseMinMax(false);
                parametr->readedValue = parametr->variable->descriptionValue();
                parametr->variable->setUseMinMax(use);
            }
        } else {
            QVariant errorDescription = m_node->lastError().description();
            for (auto parametr : readedParametrs) {
                bool use = parametr->variable->useMinMax();
                parametr->variable->setUseMinMax(false);
                parametr->readedValue = errorDescription;
                parametr->variable->setUseMinMax(use);
            }
        }
        OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Опросить по функции 0х20\""));
        m_model->updateValues();
    } else {
        ErrorClassifier::instance()->showError(4);
    }
}

void Ksu735Manual0x20TestingWidget::request0x40()
{
    Ksu735Manual0x20TestingParametrsList writedParametrs;
    QList<QPair<IVariable *, QVariant>> writedVariables;
    QList<IVariable *> readedVariables;

    for (auto parametr : m_parametrsList) {
        if (!parametr) continue;

        if (parametr->variable && parametr->enabled) {
            if (parametr->writedValue.isValid()) {
                QPair<IVariable *, QVariant> writeVariable;
                writeVariable.first = parametr->variable;
                writeVariable.second = parametr->writedValue;
                writedVariables.append(writeVariable);
            }

            writedParametrs.append(parametr);
            readedVariables.append(parametr->variable);
        }
    }

    if (!writedParametrs.isEmpty()) {
        if (!m_node->writeVariables(writedVariables, Ksu735Node::OperationTimingType::Sync)) {            
            ErrorClassifier::instance()->showError(502);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(502));
        }

        if (m_node->readVariables(readedVariables, Ksu735Node::OperationTimingType::Sync)) {
            for (auto parametr : writedParametrs) {
                bool use = parametr->variable->useMinMax();
                parametr->variable->setUseMinMax(false);
                parametr->readedValue = parametr->variable->descriptionValue();
                parametr->variable->setUseMinMax(use);
            }
        } else {
            QVariant errorDescription = m_node->lastError().description();
            for (auto parametr : writedParametrs) {
                bool use = parametr->variable->useMinMax();
                parametr->variable->setUseMinMax(false);
                parametr->readedValue = errorDescription;
                parametr->variable->setUseMinMax(use);
            }
        }
        OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Опросить по функции 0х40\""));
        m_model->updateValues();
    } else {
        ErrorClassifier::instance()->showError(4);
    }
}

void Ksu735Manual0x20TestingWidget::requestClear()
{
    for (auto parametr : m_parametrsList) {
        if (!parametr)
            continue;

        parametr->writedValue = QVariant();
        parametr->readedValue = QVariant();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Очистить запрос по функции 0х20\""));
    m_model->updateValues();
}

void Ksu735Manual0x20TestingWidget::loadSettings()
{
    QString loadFilePath = QFileDialog::getOpenFileName(this, QStringLiteral("Открыть"),
                                                        DBConfig::ksuManual0x20TestingPrestsPath(),
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

    Ksu735Manual0x20TestingParametrsList parametrsList;

    QDomElement containerElement = document.firstChildElement(KsuPresetContainerName);
    if (containerElement.isNull()) {
        ErrorClassifier::instance()->showError(47);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(47));
        return;
    }

    QDomNodeList parametrsNodes = containerElement.childNodes();
    int count = parametrsNodes.count();
    for (int i = 0; i < count; i++) {
        QDomNode parametrNode = parametrsNodes.at(i);
        if (parametrNode.isNull() || !parametrNode.isElement()) continue;

        QDomElement parametrElement = parametrNode.toElement();
        if (parametrElement.tagName() != KsuPresetParamertTag) continue;

        Ksu735Manual0x20TestingParametr *parametr = new Ksu735Manual0x20TestingParametr();
        parametrsList.append(parametr);

        bool ok;
        QString addressAttribute = parametrElement.attribute(QStringLiteral("address"));
        quint32 address = addressAttribute.toUInt(&ok);
        Ksu735Variable *variable = m_node->variableByAddress(address);

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

    for (auto parametr : m_parametrsList) {
        delete parametr;
    }
    m_parametrsList.clear();

    m_parametrsList = parametrsList;
    m_model->setParametrsList(m_parametrsList);
    OperatorClickReport::instance()->write(QString("Ручное тестирование: загружена конфигурация \"%1\"").arg(loadFilePath));
}

void Ksu735Manual0x20TestingWidget::saveSettings()
{
    QDir dir(DBConfig::ksuManual0x20TestingPrestsPath());
    dir.mkpath(dir.path());

    QString saveFilePath = QFileDialog::getSaveFileName(this,
                                                        QStringLiteral("Сохранить как"),
                                                        DBConfig::ksuManual0x20TestingPrestsPath(),
                                                        QStringLiteral("*.cfg"));

    if (saveFilePath.isEmpty())
        return;

    QDomDocument document;
    QDomProcessingInstruction instructions = document.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
    document.appendChild(instructions);

    QDomElement parametrsContainerElement = document.createElement(KsuPresetContainerName);
    document.appendChild(parametrsContainerElement);

    for (auto parametr : m_parametrsList) {
        if (!parametr) continue;

        QDomElement parametrElement = document.createElement(KsuPresetParamertTag);
        if (parametr->variable) {
            parametrElement.setAttribute(QStringLiteral("address"), parametr->variable->address());
            parametrElement.setAttribute(QStringLiteral("enabled"), parametr->enabled);
            parametrElement.setAttribute(QStringLiteral("writedValue"), parametr->writedValue.toString());
        }
        parametrsContainerElement.appendChild(parametrElement);
    }

    QFile configFile(saveFilePath);
    configFile.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream stream(&configFile);
    document.save(stream, 4);
    configFile.close();
    OperatorClickReport::instance()->write(QString("Ручное тестирование: сохранена конфигурация \"%1\"").arg(saveFilePath));
}

void Ksu735Manual0x20TestingWidget::clearSettings()
{
    for (auto parametr : m_parametrsList) {
        if (!parametr)
            continue;

        parametr->enabled = false;
        parametr->variable = nullptr;
        parametr->writedValue = QVariant();
        parametr->readedValue = QVariant();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Очистить конфигурацию 0х20\""));
    m_model->updateValues();
}
