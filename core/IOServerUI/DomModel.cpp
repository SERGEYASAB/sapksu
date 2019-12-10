#include "DomItem.h"
#include "DomModel.h"

#include "IOServer/nsNode/INode.h"
#include "IOServer/nsConnection/IConnection.h"

#include <QtXml>
#include <QIcon>
#include <QColor>

DomModel::DomModel(DomItem *rootItem, QObject *parent)
    : QAbstractItemModel(parent)
{
    m_rootItem = rootItem;
}

DomModel::~DomModel()
{
    delete m_rootItem;
}

int DomModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant DomModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    DomItem *item = static_cast<DomItem*>(index.internalPointer());

    switch (role) {

        case Qt::DisplayRole: {

            if (dynamic_cast<IConnection *>(item->ptrNode()) != nullptr) {
                return dynamic_cast<IConnection *>(item->ptrNode())->name();
            } else if (dynamic_cast<INode *>(item->ptrNode()) != nullptr) {
                return dynamic_cast<INode *>(item->ptrNode())->name();
            } else {
                return QVariant();
            }
        }

        case Qt::DecorationRole: {
            if (dynamic_cast<IConnection *>(item->ptrNode()) != nullptr) {
                const QIcon newIcon = QIcon(":/Images/gnome-fs-fifo.png");
                return newIcon;
            } else if (dynamic_cast<INode *>(item->ptrNode()) != nullptr) {
                const QIcon newIcon = QIcon(":/Images/gnome-fs-executable.png");
                return newIcon;
            }

        }

        default:
            return QVariant();

    }
}


Qt::ItemFlags DomModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant DomModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Дерево узлов");
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QModelIndex DomModel::index(int row, int column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    DomItem *childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex DomModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomItem *childItem = static_cast<DomItem*>(child.internalPointer());
    DomItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int DomModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    return parentItem == NULL ? 0 : parentItem->childCountWithoutIVariables();
}
