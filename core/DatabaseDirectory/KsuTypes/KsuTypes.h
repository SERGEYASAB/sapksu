#ifndef KSUTYPES_H
#define KSUTYPES_H

#include "Common/PrefixHeader.h"

class CustomizableTableView;

class KsuTypes : public QWidget
{
    Q_OBJECT
public:
    explicit KsuTypes(QWidget *parent = Q_NULLPTR);

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

    CustomizableTableView *m_view;
    QSqlQueryModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
};

#endif // KSUTYPES_H
