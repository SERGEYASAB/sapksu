#include "TestKit.h"

#include "DBConfig/DBConfig.h"
#include "TestKitDialogMapper.h"

void TestKit::selectRow(int row)
{
    m_view->selectRow(row);
    getIdFromRow(m_view->currentIndex());
}

TestKit::TestKit(QWidget *parent) : QWidget(parent)
{
    // создание директории
    m_testDirectory = QDir(QCoreApplication::applicationDirPath() + "/DBConfig" + "/Tests");
    m_testDirectory.mkpath(m_testDirectory.path());

    // создание модели и выборка данных
    m_model = new TestKitSqlQueryModel;
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

    QHeaderView *header = m_view->horizontalHeader();
    header->hideSection(0);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->hideSection(2);
    header->hideSection(3);
    header->hideSection(4);

    // кнопки добавить и удалить набор
    m_addButton = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));

    // добавляем кнопки в панель
    m_buttonBox = new QDialogButtonBox(Qt::Horizontal);
    m_buttonBox->addButton(m_addButton, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_removeButton, QDialogButtonBox::ActionRole);
    m_buttonBox->setCenterButtons(true);

    connect(m_view, &QTableView::doubleClicked, this, &TestKit::edit);
    connect(m_view, &QTableView::clicked, this, &TestKit::getIdFromRow);
    connect(m_addButton, &PushButtonPlus::clicked, this, &TestKit::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &TestKit::remove);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_view);
    mainLayout->addWidget(m_buttonBox);

    setLayout(mainLayout);
}

QTableView *TestKit::getView() const
{
    return m_view;
}

PushButtonPlus *TestKit::getRemoveButton() const
{
    return m_removeButton;
}

PushButtonPlus *TestKit::getAddButton() const
{
    return m_addButton;
}

int TestKit::getCurrentId() const
{
    return m_currentId;
}

QString TestKit::getCurrentName()
{
    return m_currentName;
}

void TestKit::add()
{
    TestKitDialogMapper *mapper = new TestKitDialogMapper(m_model);
    connect(mapper, &TestKitDialogMapper::dataChanged, this, &TestKit::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(QModelIndex());
    mapper->show();
}

void TestKit::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление записи");
    //    msgBox.setText(tr("Вы действительно хотите удалить запись?"));
    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());
    QString msgString = tr("Вы действительно хотите удалить набор '%1'")
                        .arg(record.value("name").toString());

    msgBox.setText(msgString);
    //    msgBox.setInformativeText(tr("Удаление протокола приведет к удалению всех связанных с ним справочников"));
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `TestKit`"
                                    "WHERE name = '%1';")
                            .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("name").toString());

        query.exec(sqlString);
        selectData();

        emit testKitDeleted();
    }
}

void TestKit::edit()
{
    TestKitDialogMapper *mapper = new TestKitDialogMapper(m_model);
    connect(mapper, &TestKitDialogMapper::dataChanged, this, &TestKit::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_proxyModel->mapToSource(m_view->currentIndex()));
    mapper->show();
}

void TestKit::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `TestKit` ORDER BY `name` ASC");
    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Название набора"));
}

int TestKit::getIdFromRow(const QModelIndex &index)
{
    m_currentId = m_model->record(m_proxyModel->mapToSource(index).row()).value("id").toInt();
    m_currentName = m_model->record(m_proxyModel->mapToSource(index).row()).value("name").toString();

    emit idClicked(m_currentId);

    return m_currentId;
}
