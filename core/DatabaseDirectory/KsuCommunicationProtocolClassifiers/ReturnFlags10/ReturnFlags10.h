#ifndef RETURNFLAGS10_H
#define RETURNFLAGS10_H

#include "Common/PrefixHeader.h"

class PushButtonPlus;
class CustomizableTableView;

class ReturnFlags10 : public QWidget
{
    Q_OBJECT
public:

    explicit ReturnFlags10(int comProtocolCode, QWidget *parent = Q_NULLPTR);

public:
    void setCurrentModelIndex(const QModelIndex &index);


private slots:
    void add();
    void remove();
    void edit();
    void selectData();

private:
    PushButtonPlus *m_editButton = nullptr;
    PushButtonPlus *m_removeButton = nullptr;

    QVBoxLayout *buttonBox = nullptr;

    CustomizableTableView *m_view = nullptr;
    QSqlQueryModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxyModel = nullptr;

    QDataWidgetMapper *m_mapper = nullptr;

    int m_comProtocolCode;

signals:
    void dataChanged();
};

#endif // RETURNFLAGS10_H
