#include "Tests.h"

#include "DBConfig/DBConfig.h"
#include "TestsDialogMapper.h"

Tests::Tests(QWidget *parent) : QWidget(parent)
{
    // создание директории
    m_testDirectory = QDir(QCoreApplication::applicationDirPath() + "/DBConfig" + "/Tests");
    m_testDirectory.mkpath(m_testDirectory.path());

    // создание модели и выборка данных
    m_model = new TestsSqlQueryModel;
    m_proxyModel = new QSortFilterProxyModel;
    m_proxyModel->setSourceModel(m_model);

    selectData();

    m_view = new QTableView;
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);
    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->verticalHeader()->setHidden(true);
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->resizeColumnsToContents();
    m_view->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_view->horizontalHeader()->setMinimumWidth(1);
    m_view->selectRow(0);

    QHeaderView *header = m_view->horizontalHeader();
    header->hideSection(0);
    header->hideSection(2);
    header->hideSection(3);
    header->hideSection(4);
    header->hideSection(5);
    header->hideSection(6);

    // кнопки добавить и удалить набор
    m_addButton = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));

    // добавляем кнопки в панель
    m_buttonBox = new QDialogButtonBox(Qt::Horizontal);
    m_buttonBox->setCenterButtons(true);


    // Отключил редактирование в данном окне
    //connect(m_view, &QTableView::doubleClicked, this, &Tests::edit);

    connect(m_view, &QTableView::clicked, this, &Tests::getIdFromRow);
    connect(m_addButton, &PushButtonPlus::clicked, this, &Tests::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &Tests::remove);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_view);
    mainLayout->addSpacing(30);
//    mainLayout->addWidget(m_buttonBox);

    // Отключил кнопки редактирования
    //    mainLayout->addWidget(buttonTypeComboBox);

    setLayout(mainLayout);
}

QTableView *Tests::getView() const
{
    return m_view;
}

int Tests::getIdFromRow(const QModelIndex &index)
{
    m_testId = m_model->record(m_proxyModel->mapToSource(index).row()).value("id").toInt();

    emit idClicked(m_testId);

    return m_testId;
}

int Tests::getCurrentId() const
{
    if (!m_view->currentIndex().isValid())
        return -1;

    return m_testId;
}

void Tests::add()
{
    m_testId = -1;

    TestsDialogMapper *mapper = new TestsDialogMapper(m_model);
    connect(mapper, &TestsDialogMapper::dataChanged, this, &Tests::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(QModelIndex());
    mapper->show();
}

void Tests::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление теста");
    //    msgBox.setText(tr("Вы действительно хотите удалить запись?"));
    QSqlRecord &&record = m_model->record(m_view->currentIndex().row());
    QString msgString = tr("Вы действительно хотите удалить тест '%1'")
                        .arg(record.value("name").toString());

    msgBox.setText(msgString);
    //    msgBox.setInformativeText(tr("Удаление протокола приведет к удалению всех связанных с ним справочников"));
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {

        QFile file(QCoreApplication::applicationDirPath() +
                   QDir::separator() + "DBConfig" +
                   QDir::separator() + "Tests" +
                   QDir::separator() +
                   m_model->record(m_view->currentIndex().row()).value("path").toString());

        file.remove();

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `Tests`"
                                    "WHERE name = '%1';")
                            .arg(m_model->record(m_view->currentIndex().row()).value("name").toString());

        query.exec(sqlString);
        selectData();

        m_testId = -1;

        emit testDeleted();
    }
}

void Tests::edit()
{
    TestsDialogMapper *mapper = new TestsDialogMapper(m_model);
    connect(mapper, &TestsDialogMapper::dataChanged, this, &Tests::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_view->currentIndex());
    mapper->show();
}

void Tests::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `Tests` ORDER BY `id` DESC");
    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Название теста"));
}
