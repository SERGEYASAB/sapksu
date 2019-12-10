#include "KSUModbusParametrsModel.h"
#include "KSUModbusParamertDomItem.h"

KSUModbusParametrsModel::KSUModbusParametrsModel(QList<KSUModbusParamertDomItem *> parametrs, QObject *parent) :
    QAbstractTableModel(parent), m_parametrs(parametrs)
{

}

QModelIndex KSUModbusParametrsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, m_parametrs[row]);
}

int KSUModbusParametrsModel::rowCount(const QModelIndex &parent) const
{
    return m_parametrs.length();
}

int KSUModbusParametrsModel::columnCount(const QModelIndex &parent) const
{
    return 9;
}

QVariant KSUModbusParametrsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    KSUModbusParamertDomItem *item = m_parametrs.at(index.row());
    if (!item) {
        return QVariant();
    }

    QDomElement domElement = item->domElement();
    if (domElement.isNull()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole: {
            if (index.column() == 1)
                return domElement.attribute(QStringLiteral("address"));
            if (index.column() == 2)
                return domElement.attribute(QStringLiteral("name"));
            if (index.column() == 3)
                return domElement.attribute(QStringLiteral("alias"));
            if (index.column() == 4)
                return domElement.attribute(QStringLiteral("deviceDataType"));
            if (index.column() == 5)
                return domElement.attribute(QStringLiteral("serverValueDataType"));
            if (index.column() == 6) {
                bool readable = domElement.attribute(QStringLiteral("readable")).toInt();
                bool writable = domElement.attribute(QStringLiteral("writable")).toInt();

                return (readable | (writable << 1));
            } if (index.column() == 7)
                return domElement.attribute(QStringLiteral("measureUnit"));
            if (index.column() == 8)
                return domElement.attribute(QStringLiteral("description"));

            break;
        }

        case Qt::ToolTipRole: {
            if (index.column() == 2 || index.column() == 3 || index.column() == 8)
                return data(index, Qt::DisplayRole);
            break;
        }

        case Qt::TextAlignmentRole: {
            return int(Qt::AlignLeft | Qt::AlignVCenter);
            break;
        }

        case Qt::EditRole: {
            if (index.column() == 6) {
                bool readable = domElement.attribute(QStringLiteral("readable")).toInt();
                bool writable = domElement.attribute(QStringLiteral("writable")).toInt();

                return (readable | (writable << 1));
            }
            break;
        }

        case Qt::CheckStateRole: {
            if (index.column() == 0) {
                if (domElement.attribute(QStringLiteral("enabled")).toInt())
                    return Qt::CheckState::Checked;
                return Qt::CheckState::Unchecked;
            }
        }
    }

    return QVariant();
}

QVariant KSUModbusParametrsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
        case Qt::ToolTipRole:
        case Qt::DisplayRole: {
            if (orientation == Qt::Horizontal) {
                if (section == 1)
                    return tr("Адрес");
                if (section == 2)
                    return tr("Название");
                if (section == 3)
                    return tr("Алиас");
                if (section == 4)
                    return tr("Тип данных устройства");
                if (section == 5)
                    return tr("Тип данных сервера");
                if (section == 6)
                    return tr("Доступ");
                if (section == 7)
                    return tr("Ед. Изм.");
                if (section == 8)
                    return tr("Описание");
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags KSUModbusParametrsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.column() == 0) {
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

void KSUModbusParametrsModel::removeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_parametrs.removeAt(row);
    endRemoveRows();
}

void KSUModbusParametrsModel::appendParametr(KSUModbusParamertDomItem *parametrItem)
{
    beginInsertRows(QModelIndex(), m_parametrs.length(), m_parametrs.length());
    m_parametrs.append(parametrItem);
    endInsertRows();
}

void KSUModbusParametrsModel::parametrItemUpdated(KSUModbusParamertDomItem *parametrItem)
{
    int row = m_parametrs.indexOf(parametrItem);
    if (row == -1) {
        return;
    }

    QModelIndex startIndex = index(row, 0);
    QModelIndex endIndex = index(row, columnCount(QModelIndex()) - 1);
    emit dataChanged(startIndex, endIndex);
}


bool KSUModbusParametrsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    KSUModbusParamertDomItem *item = m_parametrs.at(index.row());
    if (!item) {
        return false;
    }

    QDomElement domElement = item->domElement();
    if (domElement.isNull()) {
        return false;
    }

    if (index.column() == 0 && role == Qt::CheckStateRole) {
        bool data = value.toBool();
        domElement.setAttribute(QStringLiteral("enabled"), data);
        dataChanged(index, index, QVector<int>() << role);
        m_hasChages = true;
        return true;
    }

    return false;
}
