#include "ParametrAliases.h"
#include "ReportManager/FaultReport.h"
#include "DBConfig/DBConfig.h"
#include "ParametrAliasesDialogMapper.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"
#include "ErrorClassifier/ErrorClassifier.h"

ParametrAliases::ParametrAliases(QWidget *parent) : QWidget(parent)
{
    m_model = new QSqlQueryModel;
    selectData();

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    m_view = new CustomizableTableView("ParametrAliases");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

    QHeaderView *header = m_view->horizontalHeader();
    header->hideSection(0);
    header->hideSection(3);

    connect(m_view, &QTableView::doubleClicked, this, &ParametrAliases::edit);

    m_addButon = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_addButon->setDefault(true);
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));
    m_editButton= new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Редактировать"));

//    buttonBox = new QDialogButtonBox(Qt::Vertical);
//    buttonBox->addButton(m_addButon, QDialogButtonBox::ActionRole);
//    buttonBox->addButton(m_removeButton, QDialogButtonBox::ActionRole);
//    buttonBox->addButton(m_editButton, QDialogButtonBox::ActionRole);
//    connect(m_addButon, &PushButtonPlus::clicked, this, &ParametrAliases::add);
//    connect(m_removeButton, &PushButtonPlus::clicked, this, &ParametrAliases::remove);
//    connect(m_editButton, &PushButtonPlus::clicked, this, &ParametrAliases::edit);

    buttonBox = new QVBoxLayout;
    buttonBox->addWidget(m_addButon);
    buttonBox->addWidget(m_removeButton);
    buttonBox->addWidget(m_editButton);
    buttonBox->addWidget(searchBox);
    buttonBox->addStretch();
    connect(m_addButon, &PushButtonPlus::clicked, this, &ParametrAliases::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &ParametrAliases::remove);
    connect(m_editButton, &PushButtonPlus::clicked, this, &ParametrAliases::edit);

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

void ParametrAliases::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `ParametrAliases` ORDER BY `id` ASC");
    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(0, Qt::Horizontal, tr("id"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Псевдоним параметра"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Описание"));
}


void ParametrAliases::edit()
{
    if (!m_view->currentIndex().isValid()) {
        return;
    }

    //debug
#ifndef QT_NO_DEBUG
    ParametrAliasesDialogMapper *mapper = new ParametrAliasesDialogMapper(m_model);
#endif

    //release
#ifdef QT_NO_DEBUG
    ParametrAliasesDialogMapper *mapper = new ParametrAliasesDialogMapper(m_model, true);
#endif

    connect(mapper, &ParametrAliasesDialogMapper::dataChanged, this, &ParametrAliases::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_proxyModel->mapToSource(m_view->currentIndex()));
    mapper->show();
}

void ParametrAliases::add()
{
    ParametrAliasesDialogMapper *mapper = new ParametrAliasesDialogMapper(m_model);
    connect(mapper, &ParametrAliasesDialogMapper::dataChanged, this, &ParametrAliases::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(QModelIndex());
    mapper->show();
}

void ParametrAliases::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление псевдонима параметра (alias)");
    //    msgBox.setText(tr("Вы действительно хотите удалить запись?"));
    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());
    QString msgString = tr("Вы действительно хотите удалить псевдоним параметра '%1' ?")
                        .arg(record.value("alias").toString());

    msgBox.setText(msgString);
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `ParametrAliases`"
                                    "WHERE id = %1;")
                            .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("id").toString());

        db.transaction();

        if (query.exec(sqlString)) {
            if (!db.commit()){
                db.rollback();
            }

        } else {
            ErrorClassifier::instance()->showError(501);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + query.lastError().databaseText());
        }

        selectData();
    }
}

