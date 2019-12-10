#ifndef TESTS_H
#define TESTS_H

#include "Common/PrefixHeader.h"

class TestsSqlQueryModel : public QSqlQueryModel
{
public:
    explicit TestsSqlQueryModel(QObject* parent = Q_NULLPTR) : QSqlQueryModel(parent) {}

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        if (role == Qt::DecorationRole) {
            return QIcon(":/Images/oneTest.png");
        }
        else if (role == Qt::ToolTipRole) {
            return data(index, Qt::DisplayRole);
        }

        return QSqlQueryModel::data(index, role);

    }
};

class Tests : public QWidget
{
    Q_OBJECT
public:
    explicit Tests(QWidget *parent = Q_NULLPTR);

private:
    QDir m_testDirectory;

    PushButtonPlus *m_addButton;
    PushButtonPlus *m_removeButton;

    QDialogButtonBox *m_buttonBox;

    QTableView *m_view;
    QSqlQueryModel *m_model;
    QSortFilterProxyModel *m_proxyModel;

    int m_testId;

signals:
    void idClicked(int id);
    void testDeleted();

public:
    int getIdFromRow(const QModelIndex &index);
    int getCurrentId() const;

    QTableView *getView() const;

private slots:
    void selectData();
    void add();
    void remove();
    void edit();

};

#endif // TESTS_H
