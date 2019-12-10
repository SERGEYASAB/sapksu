#include "TestExternalVariableModel.h"
#include "IOServer/nsVariable/IVariable.h"

TestExternalVariableModel::TestExternalVariableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

TestExternalVariableModel::TestExternalVariableModel(const QList<IVariable *> &data, QObject *parent)
    : QAbstractTableModel(parent)
{
    setDataSource(data);
}

void TestExternalVariableModel::setDataSource(const QList<IVariable *> &data)
{
    beginResetModel();

    for (auto variable : m_data) {
        disconnect(variable, &IVariable::valueChanged, this, &TestExternalVariableModel::dataSourceDataChanged);
    }

    m_data.clear();

    if (!data.isEmpty()) {
        for (auto variable : data) {
            m_data.append(variable);

            connect(variable, &IVariable::valueChanged, this, &TestExternalVariableModel::dataSourceDataChanged);
        }
    }
    endResetModel();
}

QVariant TestExternalVariableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    IVariable *variable = m_data.at(index.row());
    if (!variable)
        return QVariant();

    Q_ASSERT(variable);

    switch (role) {

        case Qt::DisplayRole: {
            if (index.column() == 0) {
                return variable->alias();
            }

            if (index.column() == 1) {
                QVariant value = variable->value();
                if (variable->isBoolean()) {
                    return value.toBool() ? "ON" : "OFF";
                } else if (value.isValid() && !value.isNull()) {
                    QMetaType::Type type = static_cast<QMetaType::Type>(value.type());
                    if (type == QMetaType::Double || type == QMetaType::Float) {
                        return QString::number(variable->value().toReal(), 'g', 5) + " " + variable->measureUnit();
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
    }

    return QVariant();
}

int TestExternalVariableModel::rowCount(const QModelIndex &parent) const
{
    return m_data.count();
}

int TestExternalVariableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

Qt::ItemFlags TestExternalVariableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    IVariable *variable = static_cast<IVariable *>(index.internalPointer());
    Q_ASSERT(variable);
    if (!variable)
        return Qt::ItemIsEnabled;

    if (index.column() == 1 && variable->isWritable()) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return Qt::ItemIsEnabled;
}

QModelIndex TestExternalVariableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, m_data.at(row));
}

bool TestExternalVariableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        IVariable *variable = static_cast<IVariable *>(index.internalPointer());
        Q_ASSERT(variable);
        if (!variable)
            return false;

        variable->writeValue(value);
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

QVariant TestExternalVariableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return tr("Псевдоним");
        if (section == 1)
            return tr("Значение");
    }

    return QVariant();
}

void TestExternalVariableModel::dataSourceDataChanged(IVariable *variable)
{
    if (!variable) {
        return;
    }

    int row = m_data.indexOf(variable);
    if (row == -1) {
        return;
    }

    QModelIndex variableIndex = index(row, 1);
    emit dataChanged(variableIndex, variableIndex);
}
