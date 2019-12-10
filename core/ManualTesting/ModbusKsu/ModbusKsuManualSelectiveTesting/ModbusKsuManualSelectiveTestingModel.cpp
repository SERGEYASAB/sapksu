#include "ModbusKsuManualSelectiveTestingModel.h"

#include "IOServer/nsNode/ModbusKsuNode.h"
#include "IOServer/nsVariable/ModbusVariable.h"

ModbusKsuManualSelectiveTestingModel::ModbusKsuManualSelectiveTestingModel(ModbusKsuNode *node,
                                                                           ModbusKsuManualSelectiveTestingParametrsList &parametrsList,
                                                                           QObject *parent) :
    QAbstractTableModel(parent),
    m_node(node),
    m_parametrsList(parametrsList)
{
}

void ModbusKsuManualSelectiveTestingModel::setParametrsList(ModbusKsuManualSelectiveTestingParametrsList &parametrsList)
{
    m_parametrsList = parametrsList;
    updateValues();
}

ModbusKsuManualSelectiveTestingParametr *ModbusKsuManualSelectiveTestingModel::parametrByIndex(const QModelIndex &index) const
{
    return parametrByIndex(index.row());
}

void ModbusKsuManualSelectiveTestingModel::updateValues()
{
    beginResetModel();
    emit dataChanged(index(0, 0), index(m_parametrsList.count(), columnCount()));
    endResetModel();
}

void ModbusKsuManualSelectiveTestingModel::updateParametr(ModbusKsuManualSelectiveTestingParametr *parametr)
{
    int row = m_parametrsList.indexOf(parametr);

    if (row != -1) {
        beginResetModel();
        emit dataChanged(index(row, 10), index(row, columnCount() - 1));
        endResetModel();
    }
}

IVariable *ModbusKsuManualSelectiveTestingModel::variableByIndex(const QModelIndex &index) const
{
    ModbusKsuManualSelectiveTestingParametr *parametr = parametrByIndex(index);
    if (parametr) {
        return parametr->variable;
    }

    return nullptr;
}

QVariant ModbusKsuManualSelectiveTestingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Horizontal) {
        if (section == 1)
            return tr("Адрес");
        if (section == 2)
            return tr("Параметр");
        if (section == 3)
            return tr("Ед. изм.");
        if (section == 4)
            return tr("Тип данных устройства");
        if (section == 5)
            return tr("Тип данных сервера");
        if (section == 6)
            return tr("Доступ");
        if (section == 7)
            return tr("Мин");
        if (section == 8)
            return tr("Макс");
        if (section == 9)
            return tr("Умолчание");
        if (section == 10)
            return tr("Допустимое отклонение");
        if (section == 11)
            return tr("Настройка");
        if (section == 12)
            return tr("Прочитано");
    }

    return QVariant();
}

int ModbusKsuManualSelectiveTestingModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_parametrsList.size();
}

int ModbusKsuManualSelectiveTestingModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 13;
}

QVariant ModbusKsuManualSelectiveTestingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

    ModbusKsuManualSelectiveTestingParametr *parametr = parametrByIndex(index);
    if (!parametr) {
        return QVariant();
    }

    if (index.column() == 0 && role == Qt::CheckStateRole) {
        if (parametr->enabled && parametr->variable)
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
                    return parametr->variable->measureUnit();
                }

                if (index.column() == 4) {
                    return parametr->variable->deviceDataType();
                }

                if (index.column() == 5) {
                    return parametr->variable->serverValueDataType();
                }

                if (index.column() == 6) {
                    return parametr->variable->isWritable();
                }

                if (index.column() == 7) {
                    return parametr->variable->minValue();
                }

                if (index.column() == 8) {
                    return parametr->variable->maxValue();
                }

                if (index.column() == 9) {
                    if (parametr->variable->deviceDataType() == ModbusVariable::ModbusDataType::ModbusKsuDate ||
                        parametr->variable->deviceDataType() == ModbusVariable::ModbusDataType::ModbusKsuString)
                        return QVariant();
                    return parametr->variable->defaultValue();
                }

                if (index.column() == 10) {
                    return parametr->variable->permissibleVariation();
                }

                if (index.column() == 11) {
                    return parametr->writedValue;
                }

                if (index.column() == 12) {
                    return parametr->readedValue;
                }

                break;
            }

            case Qt::ToolTipRole: {
                if (index.column() == 2) {
                    return parametr->variable->name();
                }

                if (index.column() == 12) {
                    return data(index, Qt::DisplayRole);
                }

                break;
            }

            case Qt::EditRole: {
                if (index.column() == 4) {
                    return parametr->variable->deviceDataType();
                }

                if (index.column() == 5) {
                    return parametr->variable->serverValueDataType();
                }

                if (index.column() == 6) {
                    return parametr->variable->isWritable();
                }

                if (index.column() == 11) {
                    return parametr->writedValue;
                }

                break;
            }
        }
    } else {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            if (index.column() == 1) {
                return parametr->address;
            } if (index.column() == 2 && parametr->address != -1) {
                return tr("Переменной нет в классификаторе");
            }
        }
    }


    return QVariant();

}

QModelIndex ModbusKsuManualSelectiveTestingModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, m_parametrsList.at(row));

}

bool ModbusKsuManualSelectiveTestingModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (!idx.isValid())
        return false;

    ModbusKsuManualSelectiveTestingParametr *parametr = parametrByIndex(idx);
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

        parametr->address = address;

        if (address == -1) {
            parametr->variable = nullptr;
            parametr->enabled = false;
        } else {
            ModbusVariable *variable = m_node->variableByAddress(address);
            parametr->variable = variable;

            if (variable) {
                parametr->enabled = true;
            } else {
                parametr->enabled = false;
            }
        }

        parametr->writedValue = QVariant();
        parametr->readedValue = QVariant();

        emit dataChanged(index(idx.row(), 0), index(idx.row(), 12));
        return true;
    }

    if (idx.column() == 11) {
        parametr->writedValue = value;
        emit dataChanged(idx, idx);
        return true;
    }
    return false;

}

Qt::ItemFlags ModbusKsuManualSelectiveTestingModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    ModbusKsuManualSelectiveTestingParametr *parametr = parametrByIndex(index);
    if (!parametr) {
        return Qt::NoItemFlags;
    }

    if (index.column() == 11 && parametr->variable) {
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

ModbusKsuManualSelectiveTestingParametr *ModbusKsuManualSelectiveTestingModel::parametrByIndex(int index) const
{
    if (index < 0 || index > m_parametrsList.size())
        return nullptr;

    return m_parametrsList.at(index);
}
