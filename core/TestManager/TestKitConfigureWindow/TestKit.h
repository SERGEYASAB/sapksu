#ifndef TESTKIT_H
#define TESTKIT_H

#include "Common/PrefixHeader.h"

class TestKitSqlQueryModel : public QSqlQueryModel
{
public:
    explicit TestKitSqlQueryModel(QObject* parent = Q_NULLPTR) : QSqlQueryModel(parent) {}

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        if (role == Qt::DecorationRole) {
            return QIcon(":/Images/kitTest.png");
        }
        else if (role == Qt::ToolTipRole) {
            return data(index, Qt::DisplayRole);
        }

        return QSqlQueryModel::data(index, role);

    }
};

class TestKit : public QWidget
{
    Q_OBJECT
public:
    explicit TestKit(QWidget *parent = Q_NULLPTR);

private:
    QDir m_testDirectory;

    PushButtonPlus *m_addButton;
    PushButtonPlus *m_removeButton;

    QDialogButtonBox *m_buttonBox;

    QTableView *m_view;
    QSqlQueryModel *m_model;
    QSortFilterProxyModel *m_proxyModel;

    int m_currentId;
    QString m_currentName;

public:
    int getIdFromRow(const QModelIndex &index);
    void selectRow(int row);
    int getCurrentId() const;
    QString getCurrentName();

    PushButtonPlus *getAddButton() const;
    PushButtonPlus *getRemoveButton() const;

    QTableView *getView() const;

signals:
    void idClicked(int id);
    void testKitDeleted();

private slots:
    void add();
    void remove();
    void edit();
    void selectData();


};

#endif // TESTKIT_H
