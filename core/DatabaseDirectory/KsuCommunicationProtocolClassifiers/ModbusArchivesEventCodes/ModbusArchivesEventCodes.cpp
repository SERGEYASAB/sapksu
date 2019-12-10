#include "ModbusArchivesEventCodes.h"

#include "DBConfig/DBConfig.h"
#include "Common/PushButtonPlus.h"
#include "ModbusArchivesEventCodesDialogMapper.h"
#include "DatabaseDirectory/HexConvertDelegate.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

ModbusArchivesEventCodes::ModbusArchivesEventCodes(int comProtocolCode, QWidget *parent)  :
    QWidget(parent),
    m_comProtocolCode(comProtocolCode)
{
    m_model = new QSqlQueryModel;

    selectData();

    HexConvertDelegate *delegate = new HexConvertDelegate(this);

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    m_view = new CustomizableTableView("ModbusArchivesEventCodes");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);

    m_view->setItemDelegateForColumn(1, delegate);
    m_view->setColumnHidden(0, true);
    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

    connect(m_view, &QTableView::doubleClicked, this, &ModbusArchivesEventCodes::edit);

    m_addButon = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_addButon->setDefault(true);
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));

    buttonBox = new QVBoxLayout;
    buttonBox->addWidget(m_addButon);
    buttonBox->addWidget(m_removeButton);
    buttonBox->addWidget(searchBox);
    buttonBox->addStretch();

    connect(m_addButon, &PushButtonPlus::clicked, this, &ModbusArchivesEventCodes::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &ModbusArchivesEventCodes::remove);

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

void ModbusArchivesEventCodes::add()
{
    ModbusArchivesEventCodesDialogMapper *mapper = new ModbusArchivesEventCodesDialogMapper(m_comProtocolCode,
                                                                                            m_model);
    connect(mapper,     &ModbusArchivesEventCodesDialogMapper::dataChanged,
            this,       &ModbusArchivesEventCodes::selectData);

    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(QModelIndex());
    mapper->show();
}

void ModbusArchivesEventCodes::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление записи");
    msgBox.setText(tr("Вы действительно хотите удалить запись?"));

    QString msgString = QString("\"Код события\" = %1(%2)")
                        .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("code").toString())
                        .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("name").toString());

    msgBox.setInformativeText(msgString);
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `ModbusArchivesEventCodes`"
                                    "WHERE code = %1 AND comProtocolCode = %2;")
                            .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("code").toString())
                            .arg(m_comProtocolCode);

        query.exec(sqlString);
        selectData();
    }
}

void ModbusArchivesEventCodes::edit()
{
    ModbusArchivesEventCodesDialogMapper *mapper = new ModbusArchivesEventCodesDialogMapper(m_comProtocolCode,
                                                                                            m_model);
    connect(mapper,     &ModbusArchivesEventCodesDialogMapper::dataChanged,
            this,       &ModbusArchivesEventCodes::selectData);

    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_proxyModel->mapToSource(m_view->currentIndex()));
    mapper->show();
}

void ModbusArchivesEventCodes::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `ModbusArchivesEventCodes`"
                                "WHERE comProtocolCode = %1 "
                                "ORDER BY code ASC;")
                        .arg(m_comProtocolCode);

    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Код события"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Наименование события"));
}
