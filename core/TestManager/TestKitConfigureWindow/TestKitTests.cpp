#include "TestKitTests.h"

#include "DBConfig/DBConfig.h"

TestKitTests::TestKitTests(QWidget *parent) :
    QWidget(parent), m_testKitId(-1)
{
    // создание модели и выборка данных
    m_model = new TestKitTestsSqlQueryModel;

    selectData(-1);

    m_view = new QTableView;
    QSortFilterProxyModel *proxy = new QSortFilterProxyModel;
    proxy->setSourceModel(m_model);
    m_view->setModel(proxy);
    m_view->setSortingEnabled(false);
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
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->hideSection(1);
    header->hideSection(2);
    header->hideSection(3);

    // кнопки управления
    //    m_addButton = new PushButtonPlus(QIcon(":/Images/go-previous.png"), tr("Добавить"));
    //    m_removeButton = new PushButtonPlus(QIcon(":/Images/go-next.png"), tr("Удалить"));
    m_upButon = new PushButtonPlus(QIcon(":/Images/go-up.png"), tr("Вверх"));
    m_downButton = new PushButtonPlus(QIcon(":/Images/go-down.png"), tr("Вниз"));

    // добавляем кнопки в панель
    m_buttonBox = new QDialogButtonBox(Qt::Horizontal);
    //    buttonBox->addButton(m_addButton, QDialogButtonBox::ActionRole);
    //    buttonBox->addButton(m_removeButton, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_upButon, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_downButton, QDialogButtonBox::ActionRole);
    m_buttonBox->setCenterButtons(true);

    connect(m_upButon, &PushButtonPlus::clicked, this, &TestKitTests::up);
    connect(m_downButton, &PushButtonPlus::clicked, this, &TestKitTests::down);
    //    connect(m_view, &QTableView::doubleClicked, this, &TestKit::edit);
    //    connect(m_addButton, &PushButtonPlus::clicked, this, &TestKit::add);
    //    connect(m_removeButton, &PushButtonPlus::clicked, this, &TestKit::remove);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_view);
    mainLayout->addWidget(m_buttonBox);

    setLayout(mainLayout);

}

void TestKitTests::selectData(int id)
{
    m_testKitId = id;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT Tests.name, TestKitTests.*"
                                "FROM Tests INNER JOIN TestKitTests ON Tests.id = TestKitTests.testId "
                                "WHERE TestKitTests.testKitId = %1").arg(id);

    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Список тестов"));
}

void TestKitTests::up()
{
    QModelIndex currentIndex = m_view->currentIndex();
    QModelIndex swapIndex = m_model->index(currentIndex.row() - 1, currentIndex.column());

    if (!currentIndex.isValid() || !swapIndex.isValid())
        return;

    QSqlRecord currentRecord = m_model->record(currentIndex.row());
    QSqlRecord swapRecord = m_model->record(swapIndex.row());

    if (currentRecord.isEmpty() || swapRecord.isEmpty())
        return;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString updateSwapSqlString = QString("UPDATE TestKitTests SET testId = %1 WHERE id = %2;")
                                  .arg(currentRecord.value("testId").toString())
                                  .arg(swapRecord.value("id").toString());

    QString updateCurrentSqlString = QString("UPDATE TestKitTests SET testId = %1 WHERE id = %2")
                                     .arg(swapRecord.value("testId").toString())
                                     .arg(currentRecord.value("id").toString());

    QSqlQuery query(db);
    query.exec(updateSwapSqlString);
    query.exec(updateCurrentSqlString);

    selectData(m_testKitId);

    QModelIndex selectionIndex = m_model->index(swapIndex.row(), swapIndex.column());

    if (selectionIndex.isValid()) {
        m_view->selectRow(selectionIndex.row());
    }
}

void TestKitTests::down()
{
    QModelIndex currentIndex = m_view->currentIndex();
    QModelIndex swapIndex = m_model->index(currentIndex.row() + 1, currentIndex.column());

    if (!currentIndex.isValid() || !swapIndex.isValid())
        return;

    QSqlRecord currentRecord = m_model->record(currentIndex.row());
    QSqlRecord swapRecord = m_model->record(swapIndex.row());

    if (currentRecord.isEmpty() || swapRecord.isEmpty())
        return;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString updateSwapSqlString = QString("UPDATE TestKitTests SET testId = %1 WHERE id = %2;")
                                  .arg(currentRecord.value("testId").toString())
                                  .arg(swapRecord.value("id").toString());

    QString updateCurrentSqlString = QString("UPDATE TestKitTests SET testId = %1 WHERE id = %2")
                                     .arg(swapRecord.value("testId").toString())
                                     .arg(currentRecord.value("id").toString());

    QSqlQuery query(db);
    query.exec(updateSwapSqlString);
    query.exec(updateCurrentSqlString);

    selectData(m_testKitId);

    QModelIndex selectionIndex = m_model->index(swapIndex.row(), swapIndex.column());

    if (selectionIndex.isValid()) {
        m_view->selectRow(selectionIndex.row());
    }

}

void TestKitTests::remove()
{
    QModelIndex currentIndex = m_view->currentIndex();

    if (!currentIndex.isValid())
        return;

    QString sqlString = QString("DELETE FROM `TestKitTests`"
                                "WHERE id = '%1';")
                        .arg(m_model->record(m_view->currentIndex().row()).value("id").toString());

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    query.exec(sqlString);

    selectData(m_testKitId);
}
