#ifndef TESTCREATORCONTROLWINDOW_H
#define TESTCREATORCONTROLWINDOW_H

#include "Common/PrefixHeader.h"

class TestCreatorControlWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TestCreatorControlWindow(QWidget *parent = Q_NULLPTR);
    ~TestCreatorControlWindow();

private:
    QDir m_testDirectory;

    PushButtonPlus *m_exit;
    PushButtonPlus *m_add;
    PushButtonPlus *m_remove;
    PushButtonPlus *m_edit;
    PushButtonPlus *m_editor;
    PushButtonPlus *m_copy;
    PushButtonPlus *m_upload;

    QTableView *m_view;
    QSqlQueryModel *m_model;
    QSortFilterProxyModel *m_proxyModel;

    QGroupBox *m_buttonTypeFilterGroupBox;
    QButtonGroup *m_buttonTypeFilterGroup;
    QGroupBox *m_buttonProtocolFilterGroupBox;
    QButtonGroup *m_buttonProtocolFilterGroup;

    void createFilterByButtonType();
    void createFilterByProtocol();

private slots:
    void selectData();
    void remove();
    void edit();
    void add();
    void runEditor();
    void copy();
    void upload();


    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // TESTCREATORCONTROLWINDOW_H
