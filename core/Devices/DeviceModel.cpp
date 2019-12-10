#include "DeviceModel.h"

#include "IOServer/nsNode/INode.h"
#include "IOServer/nsVariable/IVariable.h"

#include <QColor>
#include <QBrush>
#include <QFont>
#include <QLocale>

DeviceModel::DeviceModel(QObject *parent) : DeviceModel(nullptr, parent)
{

}

DeviceModel::DeviceModel(const INode *node, QObject *parent) : QAbstractTableModel(parent)
{
    setDataSource(node);
}

DeviceModel::DeviceModel(const QList<IVariable *> &data, QObject *parent)
{
    setDataSource(data);
}

void DeviceModel::setDataSource(const INode *dataSource)
{
    beginResetModel();
    for (auto variable : _data) {
        disconnect(variable, &IVariable::valueChanged, this, &DeviceModel::dataSourceDataChanged);
    }
    _data.clear();

    if (dataSource) {
        for (auto variable : dataSource->variables()) {
            _data.append(variable);

            connect(variable, &IVariable::valueChanged, this, &DeviceModel::dataSourceDataChanged);
        }
    }
    endResetModel();
}

void DeviceModel::setDataSource(const QList<IVariable *> &data)
{
    beginResetModel();

    for (auto variable : _data) {
        disconnect(variable, &IVariable::valueChanged, this, &DeviceModel::dataSourceDataChanged);
    }

    _data.clear();

    if (!data.isEmpty()) {
        for (auto variable : data) {
            _data.append(variable);

            connect(variable, &IVariable::valueChanged, this, &DeviceModel::dataSourceDataChanged);
        }
    }
    endResetModel();

}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    IVariable *variable = _data.at(index.row());
    Q_ASSERT(variable);

    switch (role) {

    case Qt::DisplayRole: {
        if (index.column() == 0) {

#ifdef SAP_KSU
            return variable->name();
#endif
#ifdef SAP_SU
            return variable->name();
#endif
#ifdef MECHOS
            return variable->name() + QString(" (%1)").arg(variable->description());
#endif

        }

        if (index.column() == 1) {
            QVariant value = variable->value();
            if (variable->isBoolean()) {
                return value.toBool() ? "ON" : "OFF";
            } else if (value.isValid() && !value.isNull()) {
                QMetaType::Type type = static_cast<QMetaType::Type>(value.type());
                if (type == QMetaType::Double || type == QMetaType::Float) {
                    //                        return QLocale(QLocale::C, QLocale::UnitedStates).toString(variable->value().toReal(), 'g', 5) +
                    //                            " " + variable->measureUnit();
                    //                        return QString::number(variable->value().toReal(), 'g', 5) + " " + variable->measureUnit();
                    return SapKsu::outputToPrecision(variable->value().toReal(), variable->precision()) + " " + variable->measureUnit();
                }
                return value.toString() + " " + variable->measureUnit();
            }
        }

        break;
    }

    case Qt::ToolTipRole: {
        if (index.column() == 0)
            return variable->description();
        else if (index.column() == 1)
            return variable->descriptionValue();

        break;
    }

    case Qt::ForegroundRole: {
        if (variable->isWritable()) {
            return QVariant(QBrush(Qt::black));
        }

        if (index.column() == 0) {
            return QVariant(QBrush(Qt::darkGray));
        }
        return QVariant(QBrush(Qt::lightGray));

        break;
    }

        //        case Qt::FontRole: {
        //            QFont font = QFont();
        //            font.setBold(variable->isWritable());
        //            return QVariant(font);
        //        }

    case Qt::TextAlignmentRole: {
        if (index.column() == 0)
            return Qt::AlignVCenter;
        if (index.column() == 1)
            return Qt::AlignCenter;
        break;
    }

    case Qt::EditRole: {
        if (index.column() == 1 && variable->isWritable())
            return variable->value();
        break;
    }

    case Qt::BackgroundColorRole: {
        if (index.column() == 1 && variable->isBoolean())
            return variable->value().toBool() == false ? QColor(200, 50, 50) : QColor(50, 150, 50);
        break;
    }

        //        case Qt::SizeHintRole: {
        //            if (index.column() == 1) {
        //                return QSize(10, 10);
        //            }
        //        }
    }

    return QVariant();
}

int DeviceModel::rowCount(const QModelIndex &) const
{
    return _data.count();
}

int DeviceModel::columnCount(const QModelIndex &) const
{
    return 2;
}

Qt::ItemFlags DeviceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    IVariable *variable = static_cast<IVariable *>(index.internalPointer());
    Q_ASSERT(variable);

    if (index.column() == 1 && variable->isWritable()) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return Qt::ItemIsEnabled;
}

bool DeviceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        IVariable *variable = static_cast<IVariable *>(index.internalPointer());
        Q_ASSERT(variable);

        variable->writeValue(value);
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

QVariant DeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return tr("Channel");
        if (section == 1)
            return tr("Value");
    }

    //    if (orientation == Qt::Vertical) {
    //        return QAbstractItemModel::headerData(section, orientation, role);
    //    }

    return QVariant();
}

void DeviceModel::dataSourceDataChanged(IVariable *variable)
{
    if (!variable) {
        return;
    }

    int row = _data.indexOf(variable);
    if (row == -1) {
        return;
    }

    QModelIndex variableIndex = index(row, 1);
    emit dataChanged(variableIndex, variableIndex);
}

QModelIndex DeviceModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, _data.at(row));
}
