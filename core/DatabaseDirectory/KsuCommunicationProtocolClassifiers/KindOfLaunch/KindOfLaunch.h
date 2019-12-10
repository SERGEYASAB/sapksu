#ifndef KINDOFLAUNCH_H
#define KINDOFLAUNCH_H

#include "Common/PrefixHeader.h"

class PushButtonPlus;
class CustomizableTableView;

class KindOfLaunch : public QWidget
{
    Q_OBJECT
public:
    explicit KindOfLaunch(int comProtocolCode, QWidget *parent = Q_NULLPTR);

private slots:
    void add();
    void remove();
    void edit();
    void selectData();

private:
    PushButtonPlus *m_addButon;
    PushButtonPlus *m_removeButton;

    QVBoxLayout *buttonBox;

    CustomizableTableView *m_view;
    QSqlQueryModel *m_model;
    QSortFilterProxyModel *m_proxyModel = nullptr;

    int m_comProtocolCode;
};

#endif // KINDOFLAUNCH_H
