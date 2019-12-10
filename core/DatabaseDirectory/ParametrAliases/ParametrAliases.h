#ifndef PARAMETRALIASES_H
#define PARAMETRALIASES_H

#include "Common/PrefixHeader.h"

class CustomizableTableView;

class ParametrAliases : public QWidget
{
    Q_OBJECT
public:
    explicit ParametrAliases(QWidget *parent = Q_NULLPTR);

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

#endif // PARAMETRALIASES_H
