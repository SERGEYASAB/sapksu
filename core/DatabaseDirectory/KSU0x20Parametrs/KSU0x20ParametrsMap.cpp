#include "KSU0x20ParametrsMap.h"
#include "KSU0x20ParametrsModel.h"
#include "KSU0x20ParametrsDelegate.h"
#include "KSU0x20ParametrsFormatDelegate.h"
#include "KSU0x20ParametrsValueDelegate.h"
#include "ReportManager/FaultReport.h"

#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "Common/ParametrAliases/ParameterAliasWidgetDelegate.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

#include "ErrorClassifier/ErrorClassifier.h"
#include "ReportManager/FaultReport.h"

static const QString Sapksu735ParametsTag = "SAPKSU_735_PARAMETRS";
static const QString Sapksu0x20ParametrTag = "KSU0x20_PARAMETR";

KSU0x20ParametrsMap::KSU0x20ParametrsMap(const QString &xmlPath, QWidget *parent) :
    QWidget(parent),
    m_parametrs(QVector<KSU0x20Parametr *>(256, nullptr))
{
    m_configFile.setFileName(xmlPath);
    if (!m_configFile.open(QIODevice::ReadOnly)) {
        qCritical() << tr("Не удается открыть файл конфигурации") << m_configFile.errorString();
        return;
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    if (!m_xmlConfig.setContent(&m_configFile, &errorMsg, &errorLine, &errorColumn)) {
        qCritical() << tr("Конфигурационный файл поврежден") << errorMsg << errorLine << errorColumn;
    }
    m_configFile.close();
    traverseXml(m_xmlConfig);

    m_model = new KSU0x20ParametrsModel(m_parametrs, this);
    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    m_parametrsTable = new CustomizableTableView("KSU0x20ParametrsMap");
    m_parametrsDelegate = new KSU0x20ParametrsDelegate(this);
    m_parametrsTable->setModel(m_proxyModel);
    m_parametrsTable->setSortingEnabled(true);
//    m_parametrsTable->setItemDelegate(m_parametrsDelegate);
    //    m_parametrsTable->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_parametrsTable->setSelectionBehavior(QTableView::SelectRows);
    m_parametrsTable->setSelectionMode(QTableView::SingleSelection);
    m_parametrsTable->setItemDelegateForColumn(4, new KSU0x20ParametrsFormatDelegate(this));

    KSU0x20ParametrsValueDelegate *valueDelegate = new KSU0x20ParametrsValueDelegate(this);
    m_parametrsTable->setItemDelegateForColumn(6, valueDelegate);
    m_parametrsTable->setItemDelegateForColumn(7, valueDelegate);
    m_parametrsTable->setItemDelegateForColumn(8, valueDelegate);

    ParameterAliasWidgetDelegate *aliasDelegate = new ParameterAliasWidgetDelegate(this);
    m_parametrsTable->setItemDelegateForColumn(3, aliasDelegate);

    m_okButton = new QPushButton("Сохранить");
    m_cancelButton = new QPushButton("Закрыть");

    m_buttonBox = new QVBoxLayout();
    m_buttonBox->addWidget(searchBox);
    m_buttonBox->addStretch();
    m_buttonBox->addWidget(m_okButton);
    m_buttonBox->addWidget(m_cancelButton);

    connect(m_okButton, &PushButtonPlus::clicked, this, &KSU0x20ParametrsMap::okAction);
    connect(m_cancelButton, &PushButtonPlus::clicked, this, &KSU0x20ParametrsMap::close);


    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_parametrsTable);
    layout->addLayout(m_buttonBox);
    setLayout(layout);

    QMap<int, QString> keyValueMap;
    keyValueMap[0] = "R";
    keyValueMap[1] = "R/W";
    FKComboBoxDelegate *writeAccessDelegate = new FKComboBoxDelegate(keyValueMap, 5, this);
    m_parametrsTable->setItemDelegateForColumn(5, writeAccessDelegate);

    m_parametrsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_parametrsTable->verticalHeader()->setDefaultSectionSize(21);
    m_parametrsTable->verticalHeader()->hide();

//    QHeaderView *header = m_parametrsTable->horizontalHeader();
//    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
//    header->setSectionResizeMode(2, QHeaderView::Interactive);
//    header->setSectionResizeMode(3, QHeaderView::Interactive);
//    header->setSectionResizeMode(4, QHeaderView::Fixed);
//    header->setSectionResizeMode(5, QHeaderView::Fixed);
//    header->setSectionResizeMode(6, QHeaderView::Fixed);
//    header->setSectionResizeMode(7, QHeaderView::Fixed);
//    header->setSectionResizeMode(8, QHeaderView::Fixed);
//    header->setSectionResizeMode(9, QHeaderView::Interactive);
//    header->setSectionResizeMode(10, QHeaderView::Interactive);
//    header->setStretchLastSection(true);

//    header->setDefaultSectionSize(70);
//    header->resizeSection(9, 150);
//    header->resizeSection(10, 100);
//    header->resizeSection(2, 300);
//    header->resizeSection(3, 150);
}

void KSU0x20ParametrsMap::okAction()
{
    if (!saveConfiguration()) {
        showSaveFileErrorMessage();
        return;
    }
    m_model->resetHasChanges();
    close();
}

bool KSU0x20ParametrsMap::saveConfiguration()
{
    if (!m_configFile.open(QIODevice::WriteOnly)) {
        qCritical() << tr("Не удается сохранить изменения в файле конфигурации") << m_configFile.errorString();
        return false;
    }

    QDomDocument document = QDomDocument();
    fillXmlDocument(document);

    QTextStream stream(&m_configFile);
    document.save(stream, 4);
    m_configFile.close();

    return true;
}

void KSU0x20ParametrsMap::showSaveFileErrorMessage()
{
    ErrorClassifier::instance()->showError(45);
    FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(45));
}

void KSU0x20ParametrsMap::closeEvent(QCloseEvent *event)
{
    if (!m_model) {
        return;
    }

    if (!m_model->hasChanges()) {
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Редактирование"));
    msgBox.setText(tr("Параметры классификатора были изменены. Сохранить изменения?"));
    msgBox.setInformativeText(tr("Все несохраненные изменения будут потеряны."));
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::AcceptRole);
    QPushButton *noButton = msgBox.addButton(tr("&Нет"), QMessageBox::DestructiveRole);
    QPushButton *cancelButton = msgBox.addButton(tr("&Отмена"), QMessageBox::RejectRole);

    Q_UNUSED(cancelButton);

    msgBox.setDefaultButton(noButton);

    msgBox.exec();
    if (msgBox.clickedButton() == yesButton) {
        if (!saveConfiguration()) {
            showSaveFileErrorMessage();
            event->ignore();
        }
    } else if (msgBox.clickedButton() == cancelButton) {
        event->ignore();
    }
}

void KSU0x20ParametrsMap::traverseXml(const QDomNode &domNode)
{
    QDomNode node = domNode.firstChild();

    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement element = node.toElement();
            if (element.tagName() == Sapksu735ParametsTag) {
                QDomNodeList parametrsList = element.childNodes();
                int count = parametrsList.count();
                for (int i = 0; i < count; i++) {
                    QDomNode parametrNode = parametrsList.at(i);
                    if (parametrNode.isNull() || !parametrNode.isElement()) continue;

                    QDomElement parametrElement = parametrNode.toElement();
                    if (parametrElement.tagName() != Sapksu0x20ParametrTag) continue;

                    KSU0x20Parametr *parametr = new KSU0x20Parametr(this);

                    QDomNamedNodeMap attributesMap = parametrElement.attributes();


                    QStringList parametrAttributes = QStringList() << "address"         << "name"
                                                                   << "alias"           << "writable"
                                                                   << "enabled"         << "format"
                                                                   << "minValue"        << "maxValue"
                                                                   << "defaultValue"    << "measureUnit"
                                                                   << "permissibleVariation";

                    for (QString &attributeName : parametrAttributes) {
                        if (attributesMap.contains(attributeName)) {
                            QString attributeNodeName = attributesMap.namedItem(attributeName).nodeName();
                            parametr->setProperty(attributeNodeName.toLocal8Bit().constData(),
                                                  parametrElement.attribute(attributeNodeName));
                        }
                    }

                    quint8 address = parametr->address();
                    if (m_parametrs[address] != nullptr) {
                        delete m_parametrs[address];
                    }
                    m_parametrs[address] = parametr;
                }
            }
        }

        traverseXml(node);
        node = node.nextSibling();
    }
}

void KSU0x20ParametrsMap::fillXmlDocument(QDomDocument &document)
{
    QDomProcessingInstruction instructions = document.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
    document.appendChild(instructions);

    QDomElement parametrsContainerElement = document.createElement(Sapksu735ParametsTag);
    document.appendChild(parametrsContainerElement);


    QMetaEnum formatEnum = QMetaEnum::fromType<SapKsu::Ksu735ParametrFormat>();

    for (KSU0x20Parametr *parametr : m_parametrs) {
        if (!parametr) continue;

        QDomElement parametrElement = document.createElement(Sapksu0x20ParametrTag);
        parametrElement.setAttribute(QStringLiteral("address"), parametr->address());
        parametrElement.setAttribute(QStringLiteral("name"), parametr->name());
        parametrElement.setAttribute(QStringLiteral("alias"), parametr->alias());
        parametrElement.setAttribute(QStringLiteral("writable"), parametr->writable());
        parametrElement.setAttribute(QStringLiteral("enabled"), parametr->enabled());
        parametrElement.setAttribute(QStringLiteral("format"), formatEnum.valueToKey(parametr->format()));
        parametrElement.setAttribute(QStringLiteral("permissibleVariation"), parametr->permissibleVariation());

        if (parametr->format() == SapKsu::CCCC) {
            parametrElement.removeAttribute(QStringLiteral("minValue"));
            parametrElement.removeAttribute(QStringLiteral("maxValue"));
            parametrElement.removeAttribute(QStringLiteral("defaultValue"));
        } else {
            parametrElement.setAttribute(QStringLiteral("minValue"), parametr->minValue());
            parametrElement.setAttribute(QStringLiteral("maxValue"), parametr->maxValue());
            parametrElement.setAttribute(QStringLiteral("defaultValue"), parametr->defaultValue());
        }

        parametrElement.setAttribute("measureUnit", parametr->measureUnit());

        parametrsContainerElement.appendChild(parametrElement);

    }
}
