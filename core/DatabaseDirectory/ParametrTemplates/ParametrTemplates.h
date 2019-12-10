#ifndef PARAMETRTEMPLATES_H
#define PARAMETRTEMPLATES_H

#include "Common/PrefixHeader.h"

class ParametrTemplates: public QWidget
{
    Q_OBJECT
public:
    explicit ParametrTemplates(QWidget *parent = Q_NULLPTR);

public slots:
    void selectData();

private slots:
    void add();
    void remove();
    void edit();
    void params();

private:
    PushButtonPlus *m_addButon;
    PushButtonPlus *m_removeButton;
    PushButtonPlus *m_editButton;
    PushButtonPlus *m_paramsButton;

    QVBoxLayout *buttonBox;

    QTableView *m_view;
    QSqlQueryModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
};

#endif // PARAMETRTEMPLATES_H
