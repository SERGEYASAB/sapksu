#include "Users.h"

#include "DBConfig/DBConfig.h"
#include "UsersDialogMapper.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

Users::Users(QWidget *parent) : QWidget(parent)
{
    m_model = new QSqlQueryModel;
    selectData();

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 1);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    m_view = new CustomizableTableView("Users");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);
    m_view->setAlternatingRowColors(false);
   // m_view->verticalHeader()->hide();
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

    QHeaderView *header = m_view->horizontalHeader();
    header->hideSection(0);
//    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->hideSection(2);
    header->hideSection(3);
//    header->setSectionResizeMode(4, QHeaderView::Stretch);
//    header->setDefaultSectionSize(150);

    connect(m_view, &QTableView::doubleClicked, this, &Users::edit);

    m_addButon = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_addButon->setDefault(true);
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));
    m_editButton= new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Редактировать"));

    buttonBox = new QVBoxLayout;
    buttonBox->addWidget(m_addButon);
    buttonBox->addWidget(m_removeButton);
    buttonBox->addWidget(m_editButton);
    buttonBox->addWidget(searchBox);
    buttonBox->addStretch();

    connect(m_addButon, &PushButtonPlus::clicked, this, &Users::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &Users::remove);
    connect(m_editButton, &PushButtonPlus::clicked, this, &Users::edit);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_view);
    mainLayout->addLayout(buttonBox);
    setLayout(mainLayout);
}

void Users::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT u.*, a.name "
                                "   FROM `Users` as u "
                                "   INNER JOIN AccessLevels as a "
                                "       ON u.accessId = a.id "
                                "ORDER BY `name` ASC");

    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);

    m_model->setHeaderData(0, Qt::Horizontal, tr("id"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Имя пользователя"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Уровень доступа"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Пароль"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Уровень доступа"));
}


void Users::edit()
{
    if (!m_view->currentIndex().isValid())
        return;

    bool ok;
    int userId = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value(QStringLiteral("id")).toInt(&ok);
    if (!ok)
        return;

    UsersDialogMapper *mapper = new UsersDialogMapper(m_model, true);
    connect(mapper, &UsersDialogMapper::dataChanged, this, &Users::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setUserId(userId);
    mapper->show();
}

void Users::add()
{
    UsersDialogMapper *mapper = new UsersDialogMapper(m_model);
    connect(mapper, &UsersDialogMapper::dataChanged, this, &Users::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->show();
}

void Users::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление записи");
    msgBox.setWindowIcon(QIcon(":/Images/package_system.png"));
    //    msgBox.setText(tr("Вы действительно хотите удалить пользователя?"));
    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());
    QString msgString = tr("Вы действительно хотите удалить пользователя \r\n \"%1\"?")
            .arg(record.value("name").toString());

    msgBox.setText(msgString);
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `Users`"
                                    "WHERE id = %1;")
                .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("id").toString());

        query.exec(sqlString);
        selectData();
    }
}
