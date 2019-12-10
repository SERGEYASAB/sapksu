#include "KSUModbusParametrsMap.h"
#include "KSUModbusParametrsModel.h"
#include "KSUModbusParamertDomItem.h"
#include "ReportManager/FaultReport.h"
#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "ModbusVariableForm/ModbusVariableForm.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"
#include "ErrorClassifier/ErrorClassifier.h"
#include "ReportManager/FaultReport.h"

static const QString SapksuModbusParametrsMapNodeTag = "SAPKSU_MODBUS_PARAMETRS_MAP";
static const QString VariableNodeTagName        = "IVariable";
static const QString VariableNodeElementId      = "ModbusVariable";

KSUModbusParametrsMap::KSUModbusParametrsMap(const QString &xmlPath, QWidget *parent) :
    QWidget(parent)
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

    m_model = new KSUModbusParametrsModel(m_parametrs, this);
    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    m_parametrsTable = new CustomizableTableView("KSUModbusParametrsMap");
    m_parametrsTable->setSortingEnabled(true);
    m_parametrsTable->setModel(m_proxyModel);
    m_parametrsTable->setSelectionBehavior(QTableView::SelectRows);
    //    m_parametrsTable->setItemDelegateForColumn(4, new KSU0x20ParametrsFormatDelegate(this));

    //    KSU0x20ParametrsValueDelegate *valueDelegate = new KSU0x20ParametrsValueDelegate(this);
    //    m_parametrsTable->setItemDelegateForColumn(6, valueDelegate);
    //    m_parametrsTable->setItemDelegateForColumn(7, valueDelegate);
    //    m_parametrsTable->setItemDelegateForColumn(8, valueDelegate);

    m_okButton = new QPushButton("Сохранить");
    m_cancelButton = new QPushButton("Закрыть");

    connect(m_okButton, &PushButtonPlus::clicked, this, &KSUModbusParametrsMap::okAction);
    connect(m_cancelButton, &PushButtonPlus::clicked, this, &KSUModbusParametrsMap::close);

    PushButtonPlus *addButton = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"), this);
    addButton->setDefault(true);
    PushButtonPlus *removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"), this);
    PushButtonPlus *editButton= new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Редактировать"), this);

    QVBoxLayout *controlButtonBox = new QVBoxLayout;
    controlButtonBox->addWidget(addButton);
    controlButtonBox->addWidget(removeButton);
    controlButtonBox->addWidget(editButton);
    controlButtonBox->addWidget(searchBox);
    controlButtonBox->addStretch();
    controlButtonBox->addWidget(m_okButton);
    controlButtonBox->addWidget(m_cancelButton);

    connect(addButton, &PushButtonPlus::clicked, this, &KSUModbusParametrsMap::add);
    connect(removeButton, &PushButtonPlus::clicked, this, &KSUModbusParametrsMap::remove);
    connect(editButton, &PushButtonPlus::clicked, this, &KSUModbusParametrsMap::edit);
    connect(m_parametrsTable, &QTableView::doubleClicked, this, &KSUModbusParametrsMap::edit);

    QHBoxLayout *tableLayout = new QHBoxLayout();
    tableLayout->addWidget(m_parametrsTable);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(tableLayout);
    mainLayout->addLayout(controlButtonBox);
    setLayout(mainLayout);

    QMap<int, QString> keyValueMap;
    keyValueMap[0] = "None";
    keyValueMap[1] = "R";
    keyValueMap[2] = "W";
    keyValueMap[3] = "R/W";
    FKComboBoxDelegate *writeAccessDelegate = new FKComboBoxDelegate(keyValueMap, 6, this);
    m_parametrsTable->setItemDelegateForColumn(6, writeAccessDelegate);

    m_parametrsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_parametrsTable->verticalHeader()->setDefaultSectionSize(21);
    m_parametrsTable->verticalHeader()->hide();

    //    QHeaderView *header = m_parametrsTable->horizontalHeader();
    //    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //    header->setSectionResizeMode(1, QHeaderView::Interactive);
    //    header->setSectionResizeMode(2, QHeaderView::Interactive);
    //    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    //    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    //    header->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    //    header->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    //    header->setSectionResizeMode(7, QHeaderView::Fixed);
    //    header->setStretchLastSection(true);

    //    header->setDefaultSectionSize(70);
}

KSUModbusParametrsMap::~KSUModbusParametrsMap()
{
    qDeleteAll(m_parametrs);
    m_parametrs.clear();
}

void KSUModbusParametrsMap::okAction()
{
    if (!saveConfiguration()) {
        showSaveFileErrorMessage();
        return;
    }
    m_hasChanges = false;
    close();
}

void KSUModbusParametrsMap::add()
{
    if (m_parametrsContainerNode.isNull())
        return;

    ModbusVariableForm *addForm = new ModbusVariableForm(m_parametrsContainerNode);

    connect(addForm, &ModbusVariableForm::elementAdded, [this](QDomElement &element){
        KSUModbusParamertDomItem *item = new KSUModbusParamertDomItem(element);
        m_parametrs.append(item);
        m_model->appendParametr(item);
        m_hasChanges = true;
    });

    addForm->setAttribute(Qt::WA_DeleteOnClose, true);
    addForm->setWindowModality(Qt::ApplicationModal);

    QRect screen  = QApplication::desktop()->screenGeometry();
    addForm->move(screen.center() - addForm->rect().center());
    addForm->show();
}

void KSUModbusParametrsMap::edit()
{
    if (!m_parametrsTable->currentIndex().isValid())
        return;

    KSUModbusParamertDomItem *item = static_cast<KSUModbusParamertDomItem *>(m_proxyModel->mapToSource(m_parametrsTable->currentIndex()).internalPointer());
    QDomElement element = item->domElement();

    ModbusVariableForm *editForm = new ModbusVariableForm(element);

    connect(editForm, &ModbusVariableForm::elementSaved, [this, item](QDomElement &element) {
        Q_UNUSED(element);
        m_model->parametrItemUpdated(item);
        m_hasChanges = true;
    });

    editForm->setAttribute(Qt::WA_DeleteOnClose, true);
    editForm->setWindowModality(Qt::ApplicationModal);

    QRect screen  = QApplication::desktop()->screenGeometry();
    editForm->show();
    editForm->move(screen.center() - editForm->rect().center());
}

void KSUModbusParametrsMap::remove()
{
    if (!m_parametrsTable->currentIndex().isValid())
        return;

    KSUModbusParamertDomItem *item = static_cast<KSUModbusParamertDomItem *>(m_proxyModel->mapToSource(m_parametrsTable->currentIndex()).internalPointer());
    QDomElement element = item->domElement();

    int row = m_parametrs.indexOf(item);
    if (row == -1)
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление записи");
    //    msgBox.setText(tr("Вы действительно хотите удалить запись?"));
    QString msgString = tr("Вы действительно хотите удалить переменную \"%1\" \r\nс с адресом %2?")
                        .arg(element.attribute("name"))
                        .arg(element.attribute("address"));

    msgBox.setText(msgString);
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {
        element.parentNode().removeChild(element);
        element.clear();
        m_parametrs.removeAt(row);
        m_model->removeRow(row);

        m_hasChanges = true;
    }
}

void KSUModbusParametrsMap::closeEvent(QCloseEvent *event)
{
    if (!m_hasChanges && !m_model->hasChanges()) {
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

bool KSUModbusParametrsMap::saveConfiguration()
{
    if (!m_configFile.open(QIODevice::WriteOnly)) {
        qCritical() << tr("Не удается сохранить изменения в файле конфигурации") << m_configFile.errorString();
        return false;
    }

    QTextStream stream(&m_configFile);
    m_xmlConfig.save(stream, 4);
    m_configFile.close();

    return true;
}

void KSUModbusParametrsMap::showSaveFileErrorMessage()
{
    ErrorClassifier::instance()->showError(45);
    FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(45));
}

void KSUModbusParametrsMap::traverseXml(const QDomNode &domNode)
{
    QDomNode node = domNode.firstChild();

    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement element = node.toElement();
            if (element.tagName() == SapksuModbusParametrsMapNodeTag) {
                m_parametrsContainerNode = element;
                QDomNodeList parametrsList = element.childNodes();
                int count = parametrsList.count();
                for (int i = 0; i < count; i++) {
                    QDomNode parametrNode = parametrsList.at(i);
                    if (parametrNode.isNull() || !parametrNode.isElement()) continue;

                    QDomElement parametrElement = parametrNode.toElement();
                    if (parametrElement.tagName() != VariableNodeTagName) continue;

                    if (parametrElement.attribute(QStringLiteral("id")) != VariableNodeElementId) continue;

                    KSUModbusParamertDomItem *parametrDomItem = new KSUModbusParamertDomItem(parametrElement);

                    m_parametrs.append(parametrDomItem);
                }
            }
        }

        traverseXml(node);
        node = node.nextSibling();
    }
}
