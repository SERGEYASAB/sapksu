#include "TestKitSlectWindow.h"
#include "TestManager/TestKitConfigureWindow/TestKit.h"
#include "TestManager/ExecutorWindow/ExecutorWindow.h"
#include "TestManager/ExecutorWindow/ExecutorSimpleWindow.h"
#include "Authentication/SystemUser.h"
#include "DBConfig/DBConfig.h"
#include "IOServer/IOServer.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "IOServer/nsVariable/IVariable.h"
#include "ErrorClassifier/ErrorClassifier.h"
#include "ReportManager/FaultReport.h"

TestKitSlectWindow::TestKitSlectWindow(QWidget *parent) : QMainWindow(parent)
{
    // инициализируем базу
    m_dataBase = DBConfig::instance()->configDatabase();

    m_runTestKit = new QPushButton(tr("Выполнить"));
    connect(m_runTestKit, &QPushButton::clicked, this, &TestKitSlectWindow::runTestKit);

    m_testKit = new TestKit;
    m_testKit->getAddButton()->setHidden(true);
    m_testKit->getRemoveButton()->setHidden(true);
    m_testKit->getView()->sortByColumn(1,Qt::SortOrder::AscendingOrder);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_testKit);
    vBoxLayout->addWidget(m_runTestKit);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(vBoxLayout);
    setCentralWidget(centralWidget);
}

void TestKitSlectWindow::runTestKit()
{
    // Запись названия набора, для отчета
    //Здесь не добавляю ErrorClassifier
    //Ибо такая ошибка вылезти не может
    //А если, вдруг, вылезет - не понятно, как ее классифицировать и как решать
    if (!IOServer::instance())
        QMessageBox::critical(this, tr("Критическая ошибка"), tr("IOServer не создан"));

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT name FROM `TestKit` WHERE `id` = %1").arg(m_testKit->getCurrentId());
    QSqlQuery query(db);
    db.transaction();
    query.exec(sqlString);
    db.commit();

    if (!query.first()) {
        ErrorClassifier::instance()->showError(501);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + query.lastError().databaseText());
    }

    IOServer::instance()->getTestHistoryReport()->setTestKitName(query.value(QStringLiteral("name")).toString());

    // Список id тестов в наборе
    QList<int> testIdListOfKit = getTestIdListOfKit(m_testKit->getCurrentId());

    if (testIdListOfKit.isEmpty())
        return;

    // Список тестов
    QList<ITest *> testKit;

    for (auto testId : testIdListOfKit)
        testKit.append(TestCreator::instance()->createTest(testId));

    close();

    ExecutorWindow *testExecutor = new ExecutorWindow(testKit);
    testExecutor->setAttribute(Qt::WA_DeleteOnClose);
    testExecutor->setWindowModality(Qt::NonModal);
    testExecutor->setWindowState(Qt::WindowMaximized);

    testExecutor->setWindowTitle("Набор тестов \"" + m_testKit->getCurrentName() + "\n (режим тестирования)");

    testExecutor->setWindowIcon(QIcon(":/Images/automaticTesting.png"));
    testExecutor->move(QApplication::desktop()->screenGeometry().center() - testExecutor->rect().center());
    testExecutor->readSettings();
    testExecutor->show();
    testExecutor->activateWindow();

}

QList<int> TestKitSlectWindow::getTestIdListOfKit(int testKitId)
{
    QString sqlString = QString("SELECT Tests.id \r\n"
                                "FROM Tests INNER JOIN TestKitTests ON Tests.id = TestKitTests.testId \r\n"
                                "WHERE TestKitTests.testKitId = %1 ").arg(testKitId);

    QSqlQuery query(m_dataBase);

    QList<int> testIdListOfKit;

    if (query.exec(sqlString) == false)
    {
        ErrorClassifier::instance()->showError(501);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + query.lastError().databaseText());
    } else {

        if (query.first())
            testIdListOfKit.append(query.value("id").toInt());

        while (query.next()) {
            testIdListOfKit.append(query.value("id").toInt());
        }
    }

    return testIdListOfKit;

}
