#include "TestCreatorControlWindow.h"
#include "DBConfig/DBConfig.h"
#include "TestManager/EditorWindow/EditorWindow.h"
#include "TestManager/TestKitConfigureWindow/TestsDialogMapper.h"
#include "StartWindow/ActionControlWidgetsShow.h"
#include "TestManager/ExecutorWindow/TestCreator.h"
#include "ReportManager/OperatorClickReport.h"

void TestCreatorControlWindow::createFilterByButtonType()
{
    QRadioButton *filtredRadioButtonWithoutButton = new QRadioButton(tr("Без кнопки"));
    filtredRadioButtonWithoutButton->setChecked(true);
    QRadioButton *filtredRadioButtonWithButton = new QRadioButton(tr("С кнопкой"));
    QRadioButton *filtredRadioUniversal = new QRadioButton(tr("Универсальный"));

    m_buttonTypeFilterGroup = new QButtonGroup;
    m_buttonTypeFilterGroup->addButton(filtredRadioButtonWithoutButton);
    m_buttonTypeFilterGroup->addButton(filtredRadioButtonWithButton);
    m_buttonTypeFilterGroup->addButton(filtredRadioUniversal);

    m_buttonTypeFilterGroup->setId(filtredRadioButtonWithoutButton, 0);
    m_buttonTypeFilterGroup->setId(filtredRadioButtonWithButton, 1);
    m_buttonTypeFilterGroup->setId(filtredRadioUniversal, 2);

    m_buttonTypeFilterGroupBox = new QGroupBox(tr("Фильтр по наличию кнопки"));
    m_buttonTypeFilterGroupBox->setCheckable(true);
    m_buttonTypeFilterGroupBox->setChecked(false);

    QVBoxLayout *buttonTypeLayout = new QVBoxLayout;
    buttonTypeLayout->addWidget(filtredRadioButtonWithoutButton);
    buttonTypeLayout->addWidget(filtredRadioButtonWithButton);
    buttonTypeLayout->addWidget(filtredRadioUniversal);

    m_buttonTypeFilterGroupBox->setLayout(buttonTypeLayout);
}

void TestCreatorControlWindow::createFilterByProtocol()
{
    QRadioButton *filtredRadioButtonProtocolKSU = new QRadioButton(tr("KSU 7.35"));
    filtredRadioButtonProtocolKSU->setChecked(true);
    QRadioButton *filtredRadioButtonProtocolModbus = new QRadioButton(tr("Modbus"));
    QRadioButton *filtredRadioButtonProtocolUniversal = new QRadioButton(tr("Универсальный"));

    m_buttonProtocolFilterGroup = new QButtonGroup;
    m_buttonProtocolFilterGroup->addButton(filtredRadioButtonProtocolKSU);
    m_buttonProtocolFilterGroup->addButton(filtredRadioButtonProtocolModbus);
    m_buttonProtocolFilterGroup->addButton(filtredRadioButtonProtocolUniversal );

    m_buttonProtocolFilterGroup->setId(filtredRadioButtonProtocolKSU, 0);
    m_buttonProtocolFilterGroup->setId(filtredRadioButtonProtocolModbus, 1);
    m_buttonProtocolFilterGroup->setId(filtredRadioButtonProtocolUniversal, 2);

    m_buttonProtocolFilterGroupBox = new QGroupBox(tr("Фильтр по протоколу"));
    m_buttonProtocolFilterGroupBox->setCheckable(true);
    m_buttonProtocolFilterGroupBox->setChecked(false);

    QVBoxLayout *buttonProtocolLayout = new QVBoxLayout;
    buttonProtocolLayout->addWidget(filtredRadioButtonProtocolKSU);
    buttonProtocolLayout->addWidget(filtredRadioButtonProtocolModbus);
    buttonProtocolLayout->addWidget(filtredRadioButtonProtocolUniversal );

    m_buttonProtocolFilterGroupBox->setLayout(buttonProtocolLayout);
}

TestCreatorControlWindow::TestCreatorControlWindow(QWidget *parent) : QWidget(parent)
{
    // создание директории
    m_testDirectory = QDir(QCoreApplication::applicationDirPath() + "/DBConfig" + "/Tests");
    m_testDirectory.mkpath(m_testDirectory.path());

    // Фильтр по наличию кнопки
    createFilterByButtonType();

    // Фильтр по используемому протоколу
    createFilterByProtocol();

    // создание модели и выборка данных
    m_model = new QSqlQueryModel;
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
    m_view->sortByColumn(1, Qt::SortOrder::AscendingOrder);
    m_view->selectRow(0);

    QHeaderView *header = m_view->horizontalHeader();
    header->hideSection(0);
    header->hideSection(2);
    header->hideSection(3);
    header->hideSection(4);
    header->hideSection(5);
    header->hideSection(6);

    QSize size = QSize(32, 32);
    m_add = new PushButtonPlus(QIcon(":/Images/list-add.png"), "Создать");
    m_add->setSpaceOfIconAndText(15);
    m_add->setIconSize(size);
    m_remove = new PushButtonPlus(QIcon(":/Images/list-remove.png"), "Удалить");
    m_remove->setSpaceOfIconAndText(15);
    m_remove->setIconSize(size);
    m_edit = new PushButtonPlus(QIcon(":/Images/edit-all.png"), "Изменить");
    m_edit->setSpaceOfIconAndText(15);
    m_edit->setIconSize(size);
    m_exit = new PushButtonPlus(QIcon(":/Images/application-exit.png"), tr("Закрыть"));
    m_exit->setSpaceOfIconAndText(15);
    m_exit->setIconSize(size);
    m_editor = new PushButtonPlus(QIcon(":/Images/preferences-system.png"), tr("Открыть в редакторе"));
    m_editor->setSpaceOfIconAndText(15);
    m_editor->setIconSize(size);

    connect(m_add, &QPushButton::clicked, this, &TestCreatorControlWindow::add);
    connect(m_remove, &PushButtonPlus::clicked, this, &TestCreatorControlWindow::remove);
    connect(m_view, &QTableView::doubleClicked, this, &TestCreatorControlWindow::edit);
    connect(m_edit, &PushButtonPlus::clicked, this, &TestCreatorControlWindow::edit);
    connect(m_exit, &QPushButton::clicked, this, &TestCreatorControlWindow::close);
    connect(m_editor, &PushButtonPlus::clicked, this, &TestCreatorControlWindow::runEditor);
    connect(m_buttonTypeFilterGroupBox, &QGroupBox::clicked, this, &TestCreatorControlWindow::selectData);
    connect(m_buttonProtocolFilterGroupBox, &QGroupBox::clicked, this, &TestCreatorControlWindow::selectData);
    connect(m_buttonTypeFilterGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &TestCreatorControlWindow::selectData);
    connect(m_buttonProtocolFilterGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &TestCreatorControlWindow::selectData);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(m_add);
    vlayout->addWidget(m_remove);
    vlayout->addWidget(m_edit);
    vlayout->addWidget(m_editor);
    vlayout->addStretch(3);
    vlayout->addWidget(m_buttonTypeFilterGroupBox);
    vlayout->addStretch(1);
    vlayout->addWidget(m_buttonProtocolFilterGroupBox);
    vlayout->addStretch(2);
    vlayout->addWidget(m_exit);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_view);
    hlayout->addLayout(vlayout);

    resize(800, 600);

    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);

    const QIcon iconWindow = QIcon(":/Images/package_development.png");
    setWindowIcon(iconWindow);
    setWindowTitle(tr("\"Тестирование\""));
    setLayout(hlayout);
}

TestCreatorControlWindow::~TestCreatorControlWindow()
{

}

void TestCreatorControlWindow::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("SELECT * FROM `Tests`");

    if (m_buttonTypeFilterGroupBox->isChecked() || m_buttonProtocolFilterGroupBox->isChecked()) {
        sqlString = sqlString + " WHERE ";
    }

    if (m_buttonTypeFilterGroupBox->isChecked()) {
        sqlString = sqlString + QString(" `buttonType` = %1").arg(m_buttonTypeFilterGroup->checkedId());
    }

    if (m_buttonTypeFilterGroupBox->isChecked() && m_buttonProtocolFilterGroupBox->isChecked()) {
        sqlString = sqlString + " AND ";
    }

    if (m_buttonProtocolFilterGroupBox->isChecked()) {
        sqlString = sqlString + QString(" `protocolType` = %1").arg(m_buttonProtocolFilterGroup->checkedId());
    }

    sqlString = sqlString +  " ORDER BY `id`";

    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Название теста"));
}

void TestCreatorControlWindow::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление теста");
    //    msgBox.setText(tr("Вы действительно хотите удалить запись?"));
    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());
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
                   m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("path").toString());

        file.remove();

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `Tests`"
                                    "WHERE name = '%1';")
                .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("name").toString());

        query.exec(sqlString);
        TestCreator::instance()->removeTest(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("name").toString());
        selectData();

    }
}

void TestCreatorControlWindow::edit()
{
    TestsDialogMapper *mapper = new TestsDialogMapper(m_model);
    connect(mapper, &TestsDialogMapper::dataChanged, this, &TestCreatorControlWindow::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_proxyModel->mapToSource(m_view->currentIndex()));
    mapper->show();
}

void TestCreatorControlWindow::add()
{
    TestsDialogMapper *mapper = new TestsDialogMapper(m_model);
    connect(mapper, &TestsDialogMapper::dataChanged, this, &TestCreatorControlWindow::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(QModelIndex());
    mapper->show();

}

void TestCreatorControlWindow::runEditor()
{
    setWindowState(Qt::WindowMinimized);

    EditorWindow *editor = new EditorWindow(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("path").toString(), this);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->setWindowModality(Qt::ApplicationModal);

    editor->setWindowTitle("Тест \"" + m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("name").toString() + "\" (режим редактирования)");

    editor->setWindowIcon(QIcon(":/Images/irz_logo.png"));

    ActionControlWidgetsShow::showAdjustWidget(this, editor, QSize(1300, 900));
    editor->readSettings();
}

void TestCreatorControlWindow::copy()
{

}

void TestCreatorControlWindow::upload()
{

}

void TestCreatorControlWindow::closeEvent(QCloseEvent *event)
{
    OperatorClickReport::instance()->write(QStringLiteral("Редактор тестирования: закрытие окна"));
    QWidget::closeEvent(event);
}
