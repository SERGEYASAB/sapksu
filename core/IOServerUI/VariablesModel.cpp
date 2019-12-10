#include "VariablesModel.h"

#include "IOServer/nsNode/INode.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsConnection/IConnection.h"
#include "IOServerUI/DomItem.h"

#include <QMetaEnum>

VariablesModel::VariablesModel(DomItem *nodeItem, QObject *parent)
    : QAbstractTableModel(parent)
{
    m_domItem = nodeItem;
    m_nodeItem = dynamic_cast<INode *>(nodeItem->ptrNode());
}

VariablesModel::~VariablesModel()
{

}

QVariant VariablesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    if (!m_nodeItem)
        return QVariant();

    switch (index.column()) {
        case 0:
            return m_nodeItem->variables().at(index.row())->name();
        case 1:
            return QVariant();
//            return QMetaEnum::fromType<IVariable::DataType>().
//                    valueToKey(m_nodeItem->variables().at(index.row())->dataType());
        case 2:
            return QVariant();
//            return QMetaEnum::fromType<IVariable::ByteOrder>().
//                    valueToKey(m_nodeItem->variables().at(index.row())->byteOrder());
        case 3:
            return m_nodeItem->variables().at(index.row())->isReadable();
        case 4:
            return m_nodeItem->variables().at(index.row())->isWritable();
        case 5:
            return m_nodeItem->variables().at(index.row())->period();
        case 6:
            return m_nodeItem->variables().at(index.row())->path();
    }
}

int VariablesModel::rowCount(const QModelIndex &) const
{
    if (m_nodeItem) {
        return m_nodeItem->variables().count();
    } else {
        return 0;
    }
}

int VariablesModel::columnCount(const QModelIndex &) const
{
    if (m_nodeItem) {
        return 7;
    } else {
        return 0;
    }
}

QVariant VariablesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Имя переменной");
            case 1:
                return tr("Тип данных");
            case 2:
                return tr("Порядок байт");
            case 3:
                return tr("Чтение");
            case 4:
                return tr("Запись");
            case 5:
                return tr("Период");
            case 6:
                return tr("Путь");
            default:
                return QVariant();
        }
    }

    return QVariant();

}

QModelIndex VariablesModel::index(int row, int column, const QModelIndex &index) const
{
    if (!hasIndex(row, column))
        return QModelIndex();

    return createIndex(row, column, dynamic_cast<DomItem *>(m_domItem->child(m_domItem->childCountWithoutIVariables() + row)));
}
