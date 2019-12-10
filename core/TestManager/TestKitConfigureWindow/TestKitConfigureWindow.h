#ifndef TESTKITCONFIGUREWINDOW_H
#define TESTKITCONFIGUREWINDOW_H

#include "Common/PrefixHeader.h"

class Tests;
class TestKit;
class TestKitTests;

class TestKitConfigureWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TestKitConfigureWindow(QWidget *parent = Q_NULLPTR);

    Tests *m_test;
    TestKit *m_testKit;
    TestKitTests *m_testKitTests;

    PushButtonPlus *m_addButton;
    PushButtonPlus *m_removeButton;

    QDialogButtonBox *m_buttonBox;
    QDialogButtonBox *m_testKitTestsControllButtonBox;

    QSqlDatabase m_dataBase;

private slots:
    void addTestToKit();
    void removeTestFromKit();
    void selectTestsInKit();


    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // TESTKITCONFIGUREWINDOW_H
