#ifndef USERS_H
#define USERS_H

#include "Common/PrefixHeader.h"

class CustomizableTableView;

class Users : public QWidget
{
    Q_OBJECT
public:
    explicit Users(QWidget *parent = Q_NULLPTR);

public slots:
    void selectData();

private slots:
    void add();
    void remove();
    void edit();

private:
    PushButtonPlus *m_addButon;
    PushButtonPlus *m_removeButton;
    PushButtonPlus *m_editButton;

    QVBoxLayout *buttonBox;

    QTableView *m_view;
    QSqlQueryModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
};

#endif // USERS_H
