#ifndef TESTEXTERNALVARIABLEMODEL_H
#define TESTEXTERNALVARIABLEMODEL_H

#include "Common/PrefixHeader.h"

class IVariable;

class TestExternalVariableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TestExternalVariableModel(QObject *parent = Q_NULLPTR);
    TestExternalVariableModel(const QList<IVariable *> &data, QObject *parent = Q_NULLPTR);

    void setDataSource(const QList<IVariable *> &data);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual Qt::ItemFlags flags(const QModelIndex & index) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private slots:
    void dataSourceDataChanged(IVariable *variable);

private:
    QList<IVariable *> m_data;
};

#endif // TESTEXTERNALVARIABLEMODEL_H
