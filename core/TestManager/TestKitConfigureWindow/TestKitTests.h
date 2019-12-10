#ifndef TESTKITTESTS_H
#define TESTKITTESTS_H

#include "Common/PrefixHeader.h"

class TestKitTestsSqlQueryModel : public QSqlQueryModel
{
public:
    explicit TestKitTestsSqlQueryModel(QObject* parent = Q_NULLPTR) : QSqlQueryModel(parent) {}

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        if (role == Qt::DecorationRole) {
            if (index.row() % 2)
                return QIcon(":/Images/markerGreen.png");
            else
                return QIcon(":/Images/markerPink.png");
        }
        else if (role == Qt::ToolTipRole) {
            return data(index, Qt::DisplayRole);
        }

        return QSqlQueryModel::data(index, role);

    }
};

class TestKitTests : public QWidget
{
    Q_OBJECT
public:
    explicit TestKitTests(QWidget *parent = Q_NULLPTR);

private:

    //    PushButtonPlus *m_addButton;
    //    PushButtonPlus *m_removeButton;
    PushButtonPlus *m_upButon;
    PushButtonPlus *m_downButton;

    QDialogButtonBox *m_buttonBox;

    QTableView *m_view;
    QSqlQueryModel *m_model;

    int m_testKitId;

public slots:
    void selectData(int id);
    void up();
    void down();
    void remove();
};

#endif // TESTKITTESTS_H
