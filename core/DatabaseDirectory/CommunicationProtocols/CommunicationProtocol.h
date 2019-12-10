#ifndef COMMUNICATIONPROTOCOL_H
#define COMMUNICATIONPROTOCOL_H

#include "Common/PrefixHeader.h"

class PushButtonPlus;
class CustomizableTableView;

class CommunicationProtocol : public QWidget
{
    Q_OBJECT
public:
    explicit CommunicationProtocol(QWidget *parent = Q_NULLPTR);

public slots:
    void selectData();

private slots:
    void add();
    void remove();
    void edit();
    void params();
    void checkEnableParametrsButton();

private:
    PushButtonPlus *m_addButton;
    PushButtonPlus *m_removeButton;
    PushButtonPlus *m_editButton;
    PushButtonPlus *m_paramsButton;

    QVBoxLayout *buttonBox;

    CustomizableTableView *m_view;
    QSqlQueryModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
};

#endif // COMMUNICATIONPROTOCOL_H
