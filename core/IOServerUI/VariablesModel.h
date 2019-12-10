#ifndef VARIABLESMODEL_H
#define VARIABLESMODEL_H

#include <QObject>
#include <QAbstractTableModel>

#include "IOServer/nsNode/INode.h"
#include "IOServer/nsConnection/IConnection.h"

class DomItem;

class VariablesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit VariablesModel(DomItem *nodeItem, QObject *parent = 0);
    ~VariablesModel();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex & = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex & = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    QModelIndex index(int row, int column,
                      const QModelIndex &index = QModelIndex()) const Q_DECL_OVERRIDE;

private:
    INode *m_nodeItem;
    DomItem *m_domItem;
};

#endif // VARIABLESMODEL_H
