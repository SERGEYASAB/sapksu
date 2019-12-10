#include "ReturnFlags10.h"

#include "DBConfig/DBConfig.h"
#include "Common/PushButtonPlus.h"
#include "ReturnFlags10DialogMapper.h"
#include "DatabaseDirectory/HexConvertDelegate.h"
#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

ReturnFlags10::ReturnFlags10(int comProtocolCode, QWidget *parent) : QWidget(parent)
{
    m_comProtocolCode = comProtocolCode;

    m_model = new QSqlQueryModel;

    selectData();

    QMap<int, QString> delegateMap;
    delegateMap[0] = "XXXX";
    delegateMap[1] = "XXX.X";
    delegateMap[2] = "XX.XX";
    delegateMap[3] = "X.XXX";
    delegateMap[4] = ".XXXX";
    delegateMap[5] = "XXSS";

    FKComboBoxDelegate *formatDelegate = new FKComboBoxDelegate(delegateMap, 3, this);

    HexConvertDelegate *hexValueDelegate = new HexConvertDelegate(this);

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    m_view = new CustomizableTableView("ReturnFlags10");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);

    m_view->setItemDelegateForColumn(1, hexValueDelegate);
    m_view->setItemDelegateForColumn(3, formatDelegate);
    m_view->setColumnHidden(0, true);
    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->resizeSection(2, 350);
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

    connect(m_view, &QTableView::doubleClicked, this, &ReturnFlags10::edit);

    m_editButton = new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Изменить"));
    m_editButton->setDefault(true);
//    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));

    buttonBox = new QVBoxLayout;
    buttonBox->addWidget(m_editButton);
    buttonBox->addWidget(searchBox);
    buttonBox->addStretch();
//    buttonBox->addButton(m_removeButton, QDialogButtonBox::ActionRole);

    connect(m_editButton, &PushButtonPlus::clicked, this, &ReturnFlags10::edit);
//    connect(m_removeButton, &PushButtonPlus::clicked, this, &ReturnFlags10::remove);

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

void ReturnFlags10::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
}

void ReturnFlags10::edit()
{
    ReturnFlags10DialogMapper *mapper = new ReturnFlags10DialogMapper(m_comProtocolCode, m_model);
    connect(mapper, &ReturnFlags10DialogMapper::dataChanged, this, &ReturnFlags10::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_proxyModel->mapToSource(m_view->currentIndex()));
    mapper->show();

}

void ReturnFlags10::add()
{
    //Добавления нет в этом справочнике
//    ReturnFlags10DialogMapper *mapper = new ReturnFlags10DialogMapper(m_comProtocolCode, m_model);
//    connect(mapper, &ReturnFlags10DialogMapper::dataChanged, this, &ReturnFlags10::selectData);
//    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
//    mapper->setCurrentModelIndex(QModelIndex());
//    mapper->show();
}

void ReturnFlags10::remove()
{
    //Удаления нет в этом справочнике
//    if (!m_view->currentIndex().isValid())
//        return;

//    QMessageBox msgBox;
//    msgBox.setWindowTitle("Удаление записи");
//    msgBox.setText(tr("Вы действительно хотите удалить запись?"));

//    QString msgString = QString("\"Код\" = %1 ")
//                        .arg(m_model->record(m_view->currentIndex().row()).value("code").toString());

//    msgBox.setInformativeText(msgString);
//    msgBox.setIcon(QMessageBox::Warning);

//    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
//    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
//    msgBox.setDefaultButton(noButton);

//    msgBox.exec();

//    if (msgBox.clickedButton() == yesButton) {

//        QSqlDatabase db = DBConfig::instance()->configDatabase();
//        QSqlQuery query(db);


//        QString sqlString = QString("DELETE FROM `Return0x10FunctionParametrsFlags`"
//                                    "WHERE code = \"%1\";")
//                            .arg(m_model->record(m_view->currentIndex().row()).value("code").toString());

//        query.exec(sqlString);
//        selectData();
//    }
}

void ReturnFlags10::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `Return0x10FunctionParametrsFlags`"
                                "WHERE comProtocolCode = %1 ORDER BY code DESC;")
                        .arg(m_comProtocolCode);

    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Код"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Описание"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Тип"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Псевдоним"));
}
