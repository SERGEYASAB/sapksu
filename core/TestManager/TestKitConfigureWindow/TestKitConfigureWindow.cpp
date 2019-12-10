#include "TestKitConfigureWindow.h"
#include "DBConfig/DBConfig.h"
#include "Tests.h"
#include "TestKit.h"
#include "TestKitTests.h"
#include "TestManager/ExecutorWindow/ITest.h"
#include "TestManager/ExecutorWindow/ExecutorWindow.h"
#include "TestManager/ExecutorWindow/TestCreator.h"
#include "ReportManager/OperatorClickReport.h"
#include "ErrorClassifier/ErrorClassifier.h"
#include "ReportManager/FaultReport.h"

TestKitConfigureWindow::TestKitConfigureWindow(QWidget *parent) : QWidget(parent)
{
    // инициализируем базу
    m_dataBase = DBConfig::instance()->configDatabase();

    // инициализируем список тестов
    m_test = new Tests(this);
    m_test->getView()->sortByColumn(1,Qt::SortOrder::AscendingOrder);

    // инициализируем список наборов, по умолчанию всегда выбирается первый набор
    m_testKit = new TestKit(this);
    m_testKit->selectRow(0);
    m_testKit->getView()->sortByColumn(1,Qt::SortOrder::AscendingOrder);

    // инициализируем список тестов к наборам по выбранному id
    m_testKitTests = new TestKitTests(this);
    m_testKitTests->selectData(m_testKit->getCurrentId());

    m_addButton = new PushButtonPlus(QIcon(":/Images/go-previous.png"), tr(""));
    m_removeButton = new PushButtonPlus(QIcon(":/Images/go-next.png"), tr(""));

    m_testKitTestsControllButtonBox = new QDialogButtonBox(Qt::Vertical);
    m_testKitTestsControllButtonBox->addButton(m_addButton, QDialogButtonBox::ActionRole);
    m_testKitTestsControllButtonBox->addButton(m_removeButton, QDialogButtonBox::ActionRole);
    m_testKitTestsControllButtonBox->setCenterButtons(true);


    connect(m_testKit, &TestKit::idClicked, this, &TestKitConfigureWindow::selectTestsInKit);
    connect(m_testKit, &TestKit::testKitDeleted, this, &TestKitConfigureWindow::selectTestsInKit);
    connect(m_test, &Tests::testDeleted, this, &TestKitConfigureWindow::selectTestsInKit);
    connect(m_addButton, &PushButtonPlus::clicked, this, &TestKitConfigureWindow::addTestToKit);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &TestKitConfigureWindow::removeTestFromKit);

    QFont boldFont;
    boldFont.setBold(true);

    QVBoxLayout *testKitLayout = new QVBoxLayout;
    testKitLayout->addWidget(m_testKit);

    QVBoxLayout *testKitTestsLayout = new QVBoxLayout;
    testKitTestsLayout->addWidget(m_testKitTests);

    QVBoxLayout *testKitTestButtonLayout = new QVBoxLayout;
    testKitTestButtonLayout->addWidget(m_testKitTestsControllButtonBox);

    QVBoxLayout *testLayout = new QVBoxLayout;
    testLayout->addWidget(m_test);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addLayout(testKitLayout);
    hlayout->addLayout(testKitTestsLayout);
    hlayout->addLayout(testKitTestButtonLayout);
    hlayout->addLayout(testLayout);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(new QLabel(tr("\rУправление наборами тестов:\n"
                                        "\rВ каждый набор может входить любое количество тестов несколько раз.\n"
                                        "\rПоследовательность выполняемых тестов может быть изменена.")));
    vBoxLayout->addLayout(hlayout);

    setLayout(vBoxLayout);
}

void TestKitConfigureWindow::addTestToKit()
{
    int testId = m_test->getCurrentId();
    int testKitId = m_testKit->getCurrentId();

    if (testId < 0 || testKitId < 0)
        return;

    QString sqlString;


    sqlString = QString("INSERT INTO `TestKitTests` (`testKitId`, `testId`)"
                        "VALUES (%1, %2)").arg(testKitId).arg(testId);

    QSqlQuery query(m_dataBase);

    if (query.exec(sqlString) == false)
    {
        ErrorClassifier::instance()->showError(501);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + query.lastError().databaseText());
    } else {
        m_testKitTests->selectData(testKitId);
    }
}

void TestKitConfigureWindow::removeTestFromKit()
{
    m_testKitTests->remove();
}

void TestKitConfigureWindow::selectTestsInKit()
{
    m_testKitTests->selectData(m_testKit->getCurrentId());
}

void TestKitConfigureWindow::closeEvent(QCloseEvent *event)
{
    OperatorClickReport::instance()->write(QStringLiteral("Редактор наборов тестов: закрытие окна"));
    QWidget::closeEvent(event);
}
