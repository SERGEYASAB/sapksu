#ifndef SUTYPES_H
#define SUTYPES_H

#include "Common/PrefixHeader.h"

class CustomizableTableView;

class SUTypes : public QWidget
{
    Q_OBJECT
public:
    explicit SUTypes(QWidget *parent = Q_NULLPTR);

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

private:
    void showQueryError(const QSqlQuery &query);
};

#endif // SUTYPES_H
