#ifndef TestKitSlectWindow_copy_H
#define TestKitSlectWindow_copy_H

#include "Common/PrefixHeader.h"
#include "TestManager/ExecutorWindow/TestCreator.h"

class TestKit;
class TestKitSlectWindow_copy : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestKitSlectWindow_copy(QWidget *parent = Q_NULLPTR);

    TestKit *m_testKit;
    QSqlDatabase m_dataBase;

    QPushButton *m_runTestKit;
    QPushButton *m_runByStepTestKit;

private:
    void runTestKit();
     bool m_paused = false;
    QList<int> getTestIdListOfKit(int testKitId);
};



#endif // TestKitSlectWindow_copy_H
