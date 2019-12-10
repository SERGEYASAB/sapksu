#ifndef TESTINTERNALVARIABLEMODEL_H
#define TESTINTERNALVARIABLEMODEL_H

#include "Common/PrefixHeader.h"

class EngineLocalVariable;

class TestInternalVariableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TestInternalVariableModel(QObject *parent = Q_NULLPTR);
    TestInternalVariableModel(const QList<EngineLocalVariable *> &data, QObject *parent = Q_NULLPTR);

    void setDataSource(const QList<EngineLocalVariable *> &data);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual Qt::ItemFlags flags(const QModelIndex & index) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private slots:
    void dataSourceDataChanged(EngineLocalVariable *variable);

private:
    QList<EngineLocalVariable *> m_data;
};

#endif // TESTINTERNALVARIABLEMODEL_H
