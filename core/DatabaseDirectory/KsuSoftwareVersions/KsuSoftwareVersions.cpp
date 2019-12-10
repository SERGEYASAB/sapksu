#include "KsuSoftwareVersions.h"

#include "DBConfig/DBConfig.h"
#include "KsuSoftwareVersionDialogMapper.h"
#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "DatabaseDirectory/KSU0x20Parametrs/KSU0x20ParametrsMap.h"
#include "DatabaseDirectory/KSUModbusParametrs/KSUModbusParametrsMap.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

KsuSoftwareVersions::KsuSoftwareVersions(QWidget *parent) : QWidget(parent)
{
    m_model = new QSqlQueryModel(this);
    selectData();

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    QMap<int, QString> delegateMap;
    delegateMap[0] = "7.35 1B";
    delegateMap[1] = "Modbus";
    FKComboBoxDelegate *delegate = new FKComboBoxDelegate(delegateMap, this);

    m_view = new CustomizableTableView("KsuSoftwareVersions");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);
    m_view->setItemDelegateForColumn(3, delegate);
//    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

#ifdef QT_NO_DEBUG
    m_view->hideColumn(0);
    m_view->hideColumn(6);
#endif

    connect(m_view, &QTableView::doubleClicked, this, &KsuSoftwareVersions::edit);
    //    connect(m_view, &QTableView::clicked, this, &CommunicationProtocol::checkEnableParametrsButton);

    m_addButton = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_addButton->setDefault(true);
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));
    m_editButton= new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Редактировать"));
    m_paramsButton = new PushButtonPlus(QIcon(":/Images/document-config.png"), tr("Параметры"));

    buttonBox = new QVBoxLayout();
    buttonBox->addWidget(m_addButton);
    buttonBox->addWidget(m_removeButton);
    buttonBox->addWidget(m_editButton);
    buttonBox->addWidget(m_paramsButton);
    buttonBox->addWidget(searchBox);
    buttonBox->addStretch();

    connect(m_addButton, &PushButtonPlus::clicked, this, &KsuSoftwareVersions::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &KsuSoftwareVersions::remove);
    connect(m_editButton, &PushButtonPlus::clicked, this, &KsuSoftwareVersions::edit);
    connect(m_paramsButton, &PushButtonPlus::clicked, this, &KsuSoftwareVersions::params);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_view);
    mainLayout->addLayout(buttonBox);
    setLayout(mainLayout);
    resize(1024, 768);
    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowTitle(tr("Справочник протоколов"));
}

void KsuSoftwareVersions::add()
{
    KsuSoftwareVersionDialogMapper *mapper = new KsuSoftwareVersionDialogMapper();
    connect(mapper, &KsuSoftwareVersionDialogMapper::dataChanged, this, &KsuSoftwareVersions::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->show();
}

void KsuSoftwareVersions::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());
    bool ok;
    int softwareVersionId = record.value(QStringLiteral("id")).toInt(&ok);

    if (!ok || record.isEmpty())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление версии ПО КСУ");
    QString msgString = tr("Вы действительно хотите удалить версию ПО КСУ \"%1\"?")
                        .arg(record.value(QStringLiteral("softwareVersion")).toString());

    msgBox.setText(msgString);
    msgBox.setInformativeText(tr("Удаление версии ПО КСУ приведет к удалению пользовательского справочника параметров"));
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {
        QString fileName = record.value(QStringLiteral("parametrsConfigPath")).toString();

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);

        QString sqlString = QString("DELETE FROM `KsuSoftwareVersions` WHERE id = %1;")
                            .arg(softwareVersionId);

        if (query.exec(sqlString)) {
            QFile::remove(QDir(DBConfig::ksuParametrsFolderPath()).absoluteFilePath(fileName));
        }
        selectData();
    }
}

void KsuSoftwareVersions::edit()
{
    if (!m_view->currentIndex().isValid())
        return;

    bool ok;
    int softwareVersionId = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value(QStringLiteral("id")).toInt(&ok);
    if (!ok)
        return;

    KsuSoftwareVersionDialogMapper *mapper = new KsuSoftwareVersionDialogMapper();
    connect(mapper, &KsuSoftwareVersionDialogMapper::dataChanged, this, &KsuSoftwareVersions::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setKSUSoftwareVersionId(softwareVersionId);
    mapper->show();
}

void KsuSoftwareVersions::params()
{
    if (!m_view->currentIndex().isValid())
        return;

    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());
    if (record.isEmpty())
        return;

    QString parametrsXMLFileName = record.value(QStringLiteral("parametrsConfigPath")).toString();
    if (parametrsXMLFileName.isEmpty())
        return;

    QString parametrsXMLFilePath = QDir(DBConfig::ksuParametrsFolderPath()).absoluteFilePath(parametrsXMLFileName);

    QWidget *parametrsWidget = nullptr;
    if (record.value(QStringLiteral("protocolType")) == 0) {
        parametrsWidget = new KSU0x20ParametrsMap(parametrsXMLFilePath);
    } else {
        parametrsWidget = new KSUModbusParametrsMap(parametrsXMLFilePath);
    }

    parametrsWidget->setAttribute(Qt::WA_DeleteOnClose, true);
    parametrsWidget->setWindowModality(Qt::ApplicationModal);
    parametrsWidget->resize(1024, 768);

    QRect screen  = QApplication::desktop()->screenGeometry();
    parametrsWidget->move(screen.center() - rect().center());
    parametrsWidget->show();
}

void KsuSoftwareVersions::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT "
                                "sv.id, "
                                "t.ksuName, "
                                "sv.softwareVersion,"
                                "sv.protocolType,"
                                "p.name,"
                                "coalesce(sv.parametrsClassifierName, pt.name, 'Пользовательский классификатор') AS classifierName,"
                                "sv.parametrsConfigPath"
                                "   FROM KsuSoftwareVersions AS sv "
                                "   INNER JOIN KsuTypes AS t "
                                "       ON sv.ksuTypeId = t.id "
                                "   INNER JOIN CommunicationProtocols AS p "
                                "       ON sv.comProtocolCode = p.code AND sv.protocolType = p.protocolType"
                                "   LEFT OUTER JOIN KSUClassifierParametersTemplates AS pt"
                                "       ON sv.parametrsTemplateId = pt.id");
    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Id"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Тип КСУ"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Версия ПО"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Тип протокола"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Протокол"));
    m_model->setHeaderData(5, Qt::Horizontal, tr("Классификатор"));
    m_model->setHeaderData(6, Qt::Horizontal, tr("Путь конфигурации"));
}
