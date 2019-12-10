#ifndef KSUSOFTWAREVERSIONS_H
#define KSUSOFTWAREVERSIONS_H

#include "Common/PrefixHeader.h"

class CustomizableTableView;

class KsuSoftwareVersions : public QWidget
{
    Q_OBJECT
public:
    explicit KsuSoftwareVersions(QWidget *parent = 0);

public slots:
    void selectData();

private slots:
    void add();
    void remove();
    void edit();
    void params();

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

#endif // KSUSOFTWAREVERSIONS_H
