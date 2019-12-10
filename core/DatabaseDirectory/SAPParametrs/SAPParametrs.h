#ifndef SAPPARAMETRS_H
#define SAPPARAMETRS_H

#include "Common/PrefixHeader.h"

class CustomizableTableView;

class SAPParametrs: public QWidget
{
    Q_OBJECT
public:

    explicit SAPParametrs(QWidget *parent = Q_NULLPTR);

public slots:
    void selectData();

private slots:
    void add();
    void remove();
    void edit();
    void params();

private:
    PushButtonPlus *m_addButon = nullptr;
    PushButtonPlus *m_removeButton = nullptr;
    PushButtonPlus *m_editButton = nullptr;
    PushButtonPlus *m_paramsButton = nullptr;

    QVBoxLayout *buttonBox = nullptr;

    CustomizableTableView *m_view = nullptr;
    QSqlQueryModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxyModel = nullptr;
};

#endif // SAPPARAMETRS_H
