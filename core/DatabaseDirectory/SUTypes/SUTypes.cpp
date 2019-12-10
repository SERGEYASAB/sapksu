#include "SUTypes.h"
#include "ReportManager/FaultReport.h"
#include "DBConfig/DBConfig.h"
#include "SUTypesDialogMapper.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"
#include "ErrorClassifier/ErrorClassifier.h"

SUTypes::SUTypes(QWidget *parent) : QWidget(parent)
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

    m_view = new CustomizableTableView("SUTypes");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

    QHeaderView *header = m_view->horizontalHeader();
    header->hideSection(0);
//    header->setSortIndicatorShown(false);
//    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
//    header->setSectionResizeMode(2, QHeaderView::Stretch);
//    header->setSectionResizeMode(3, QHeaderView::Stretch);
//    header->setDefaultSectionSize(40);

    connect(m_view, &QTableView::doubleClicked, this, &SUTypes::edit);

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

    connect(m_addButon, &PushButtonPlus::clicked, this, &SUTypes::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &SUTypes::remove);
    connect(m_editButton, &PushButtonPlus::clicked, this, &SUTypes::edit);

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

void SUTypes::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `SUTypes` ORDER BY `code` ASC");

    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Код"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Тип СУ"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Завод-изготовитель"));
}


void SUTypes::edit()
{
    if (!m_view->currentIndex().isValid())
        return;

    SUTypesDialogMapper *mapper = new SUTypesDialogMapper(m_model);
    connect(mapper, &SUTypesDialogMapper::dataChanged, this, &SUTypes::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_proxyModel->mapToSource(m_view->currentIndex()));
    mapper->show();
}

void SUTypes::add()
{
    SUTypesDialogMapper *mapper = new SUTypesDialogMapper(m_model);
    connect(mapper, &SUTypesDialogMapper::dataChanged, this, &SUTypes::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->show();
}

void SUTypes::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление записи");
    //    msgBox.setText(tr("Вы действительно хотите удалить запись?"));
    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());
    QString msgString = tr("Вы действительно хотите удалить тип СУ \"%1\" \r\nс кодом %2?")
                        .arg(record.value("SUName").toString())
                        .arg(record.value("code").toString());

    msgBox.setText(msgString);
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `SUTypes`"
                                    "WHERE code = \"%1\";")
                            .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("code").toString());

        if (!query.exec(sqlString)) {
            ErrorClassifier::instance()->showError(501);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + query.lastError().databaseText());
        }
        selectData();
    }
}
