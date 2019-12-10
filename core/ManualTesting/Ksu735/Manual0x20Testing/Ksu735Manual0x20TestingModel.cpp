#include "Ksu735Manual0x20TestingModel.h"

#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

Ksu735Manual0x20TestingModel::Ksu735Manual0x20TestingModel(Ksu735Node *node,
                                                           Ksu735Manual0x20TestingParametrsList &parametrsList,
                                                           QObject *parent) :
    QAbstractTableModel(parent),
    m_node(node),
    m_parametrsList(parametrsList)
{
}

void Ksu735Manual0x20TestingModel::setParametrsList(Ksu735Manual0x20TestingParametrsList &parametrsList)
{
    m_parametrsList = parametrsList;
    updateValues();
}

Ksu735Manual0x20TestingParametr *Ksu735Manual0x20TestingModel::parametrByIndex(const QModelIndex &index) const
{
    if (index.row() >= 0 && index.row() < m_parametrsList.count()) {
        return m_parametrsList.at(index.row());
    }
    return nullptr;
}

void Ksu735Manual0x20TestingModel::updateValues()
{
    beginResetModel();
    emit dataChanged(index(0, 0), index(m_parametrsList.count(), columnCount()));
    endResetModel();
}

QVariant Ksu735Manual0x20TestingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Horizontal) {
        if (section == 1)
            return tr("Код");
        if (section == 2)
            return tr("Параметр");
        if (section == 3)
            return tr("Формат");
        if (section == 4)
            return tr("Доступ");
        if (section == 5)
            return tr("Мин");
        if (section == 6)
            return tr("Макс");
        if (section == 7)
            return tr("Умолчание");
        if (section == 8)
            return tr("Допустимое отклонение");
        if (section == 9)
            return tr("Настройка");
        if (section == 10)
            return tr("Прочитано");
    }

    return QVariant();
}

int Ksu735Manual0x20TestingModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_parametrsList.size();
}

int Ksu735Manual0x20TestingModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 11;
}

QVariant Ksu735Manual0x20TestingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

    Ksu735Manual0x20TestingParametr *parametr = m_parametrsList.at(index.row());
    if (!parametr) {
        return QVariant();
    }

    if (index.column() == 0 && role == Qt::CheckStateRole) {
        if (parametr->enabled)
            return Qt::CheckState::Checked;
        return Qt::CheckState::Unchecked;
    }

    if (parametr->variable) {
        switch (role) {
            case Qt::DisplayRole: {
                if (index.column() == 1) {
                    return parametr->variable->address();
                }

                if (index.column() == 2) {
                    return parametr->variable->name();
                }

                if (index.column() == 3) {
                    return parametr->variable->format();
                }

                if (index.column() == 4) {
                    return parametr->variable->isWritable();
                }

                if (index.column() == 5) {
                    return parametr->variable->minValue();
                }

                if (index.column() == 6) {
                    return parametr->variable->maxValue();
                }

                if (index.column() == 7) {
                    return parametr->variable->defaultValue();
                }

                if (index.column() == 8) {
                    return parametr->variable->permissibleVariation();
                }

                if (index.column() == 9) {
                    return parametr->writedValue;
                }

                if (index.column() == 10) {
                    return parametr->readedValue;
                }

                break;
            }

            case Qt::ToolTipRole: {
                if (index.column() == 2) {
                    return parametr->variable->name();
                }

                if (index.column() == 8) {
                    return parametr->variable->permissibleVariation();
                }

                if (index.column() == 10) {
                    return parametr->readedValue;
                }

                break;
            }

            case Qt::EditRole: {
                if (index.column() == 3) {
                    return parametr->variable->format();
                }

                if (index.column() == 4) {
                    return parametr->variable->isWritable();
                }

                if (index.column() == 8) {
                    return parametr->writedValue;
                }

                break;
            }
        }
    } else {
        if (index.column() == 1 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
            return -1;
        }
    }


    return QVariant();
}

QModelIndex Ksu735Manual0x20TestingModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, m_parametrsList.at(row));
}

bool Ksu735Manual0x20TestingModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (!idx.isValid())
        return false;

    Ksu735Manual0x20TestingParametr *parametr = m_parametrsList.at(idx.row());
    if (!parametr) {
        return false;
    }

    if (idx.column() == 0 && role == Qt::CheckStateRole) {
        bool data = value.toBool();
        parametr->enabled = data;
        emit dataChanged(idx, idx, QVector<int>() << role);
        return true;
    }

    if (idx.column() == 1) {
        bool ok;
        int address = value.toInt(&ok);
        if (!ok)
            return false;

        if (address == -1) {
            parametr->variable = nullptr;
            parametr->enabled = false;
        } else {
            Ksu735Variable *variable = m_node->variableByAddress(address);
            parametr->variable = variable;
            parametr->enabled = true;
        }

        parametr->writedValue = QVariant();
        parametr->readedValue = QVariant();

        emit dataChanged(index(idx.row(), 0), index(idx.row(), 9));
        return true;
    } else if (idx.column() == 9) {
        parametr->writedValue = value;
        emit dataChanged(idx, idx);
        return true;
    }
    return false;
}

Qt::ItemFlags Ksu735Manual0x20TestingModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    Ksu735Manual0x20TestingParametr *parametr = m_parametrsList.at(index.row());
    if (!parametr) {
        return Qt::NoItemFlags;
    }

    if (index.column() == 9 && m_parametrsList.at(index.row())->variable) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemNeverHasChildren;
    }

    if (index.column() == 0) {
        Qt::ItemFlags flags =  Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
        if (parametr->variable) {
            flags |= Qt::ItemIsEnabled;
        }
        return flags;
    } else if (index.column() == 1) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemNeverHasChildren;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}
