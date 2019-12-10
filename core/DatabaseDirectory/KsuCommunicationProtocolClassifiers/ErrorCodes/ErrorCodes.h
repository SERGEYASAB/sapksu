#ifndef ERRORCODES_H
#define ERRORCODES_H

#include "Common/PrefixHeader.h"

class PushButtonPlus;
class CustomizableTableView;

class ErrorCodes : public QWidget
{
    Q_OBJECT
public:

    explicit ErrorCodes(int comProtocolCode, QWidget *parent = Q_NULLPTR);

private slots:
    void add();
    void remove();
    void edit();
    void selectData();

private:
    PushButtonPlus *m_addButon = nullptr;
    PushButtonPlus *m_removeButton = nullptr;

    QVBoxLayout *buttonBox = nullptr;

    CustomizableTableView *m_view = nullptr;
    QSqlQueryModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxyModel = nullptr;

    int m_comProtocolCode;
};

#endif // ERRORCODES_H
