#ifndef TESTKITSLECTWINDOW_H
#define TESTKITSLECTWINDOW_H

#include "Common/PrefixHeader.h"
#include "TestManager/ExecutorWindow/TestCreator.h"

class TestKit;
class TestKitSlectWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestKitSlectWindow(QWidget *parent = Q_NULLPTR);

    TestKit *m_testKit;
    QSqlDatabase m_dataBase;

    QPushButton *m_runTestKit;
    QPushButton *m_runByStepTestKit;

private:
    void runTestKit();

    QList<int> getTestIdListOfKit(int testKitId);
};

#endif // TESTKITSLECTWINDOW_H
