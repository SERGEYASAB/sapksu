#include "ParametrTemplates.h"

#include "DBConfig/DBConfig.h"
#include "ParametrTemplatesDialogMapper.h"
#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "DatabaseDirectory/KSU0x20Parametrs/KSU0x20ParametrsMap.h"
#include "DatabaseDirectory/KSUModbusParametrs/KSUModbusParametrsMap.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

ParametrTemplates::ParametrTemplates(QWidget *parent) : QWidget(parent)
{
    m_model = new QSqlQueryModel;
    selectData();

    QMap<int, QString> delegateMap;
    delegateMap[0] = "7.35 1B";
    delegateMap[1] = "Modbus";

    FKComboBoxDelegate *delegate = new FKComboBoxDelegate(delegateMap, this);

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 3);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    m_view = new CustomizableTableView("ParametrTemplates");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);
    m_view->setItemDelegateForColumn(2, delegate);
//    m_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
//    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->horizontalHeader()->hideSection(0);

    //release
#ifdef QT_NO_DEBUG
    m_view->horizontalHeader()->hideSection(4);
#endif

    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

    connect(m_view, &QTableView::doubleClicked, this, &ParametrTemplates::edit);

    m_addButon = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_addButon->setDefault(true);
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));
    m_editButton= new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Редактировать"));
    m_paramsButton = new PushButtonPlus(QIcon(":/Images/document-config.png"), tr("Параметры"));

    buttonBox = new QVBoxLayout;
    buttonBox->addWidget(m_addButon);
    buttonBox->addWidget(m_removeButton);
    buttonBox->addWidget(m_editButton);
    buttonBox->addWidget(m_paramsButton);
    buttonBox->addWidget(searchBox);
    buttonBox->addStretch();

    connect(m_addButon, &PushButtonPlus::clicked, this, &ParametrTemplates::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &ParametrTemplates::remove);
    connect(m_editButton, &PushButtonPlus::clicked, this, &ParametrTemplates::edit);
    connect(m_paramsButton, &PushButtonPlus::clicked, this, &ParametrTemplates::params);

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

void ParametrTemplates::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `KSUClassifierParametersTemplates` ORDER BY `code` ASC");
    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(0, Qt::Horizontal, tr("id"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Код"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Протокол"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Название"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Путь конфигурации"));
}

void ParametrTemplates::add()
{
    ParametrTemplatesDialogMapper *mapper = new ParametrTemplatesDialogMapper(m_model);
    connect(mapper, &ParametrTemplatesDialogMapper::dataChanged, this, &ParametrTemplates::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(QModelIndex());
    mapper->show();
}

void ParametrTemplates::edit()
{
    ParametrTemplatesDialogMapper *mapper = new ParametrTemplatesDialogMapper(m_model, true);
    connect(mapper, &ParametrTemplatesDialogMapper::dataChanged, this, &ParametrTemplates::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_proxyModel->mapToSource(m_view->currentIndex()));
    mapper->show();
}

void ParametrTemplates::params()
{
    if (!m_view->currentIndex().isValid())
        return;

    QSqlRecord record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());

    if (record.isEmpty())
        return;

    QString parametrsXMLFileName = record.value("parametrsConfigPath").toString();
    if (parametrsXMLFileName.isEmpty())
        return;

    QString parametrsXMLFilePath = DBConfig::parametrsTemplatesFolderPath() + QDir::separator() +
                                   parametrsXMLFileName;

    QWidget *parametrsWidget = nullptr;
    if (record.value("protocolType") == 0) {
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

void ParametrTemplates::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление шаблона классификатора");
    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());
    QString msgString = tr("Вы действительно хотите удалить шаблон \"%1\" \r\nс кодом %2?")
                        .arg(record.value("name").toString())
                        .arg(record.value("code").toString());

    msgBox.setText(msgString);
    msgBox.setInformativeText(tr("Удаление шаблона приведет к удалению всех связанных с ним справочников"));
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {

        QFile file(DBConfig::parametrsTemplatesFolderPath() + QDir::separator() +
                   record.value("parametrsConfigPath").toString());

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `KSUClassifierParametersTemplates`"
                                    "WHERE code = \"%1\";")
                            .arg(record.value("code").toString());

        if (query.exec(sqlString)) {
            file.remove();
        }
        selectData();
    }
}
