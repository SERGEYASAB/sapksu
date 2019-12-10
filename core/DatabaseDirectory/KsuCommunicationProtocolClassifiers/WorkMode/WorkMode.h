#ifndef WORKMODE_H
#define WORKMODE_H

#include "Common/PrefixHeader.h"

class PushButtonPlus;
class CustomizableTableView;

class WorkMode : public QWidget
{
    Q_OBJECT
public:

    explicit WorkMode(int comProtocolCode, QWidget *parent = Q_NULLPTR);

private slots:
    void add();
    void remove();
    void edit();
    void selectData();

private:
    PushButtonPlus *m_addButon;
    PushButtonPlus *m_removeButton;

    QVBoxLayout *buttonBox = nullptr;

    CustomizableTableView *m_view = nullptr;
    QSqlQueryModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxyModel = nullptr;

    int m_comProtocolCode;
};

#endif // WORKMODE_H
