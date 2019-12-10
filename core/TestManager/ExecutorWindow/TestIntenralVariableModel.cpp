#include "TestInternalVariableModel.h"
#include "TestManager/EditorWindow/EngineLocalVariable.h"

TestInternalVariableModel::TestInternalVariableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

TestInternalVariableModel::TestInternalVariableModel(const QList<EngineLocalVariable *> &data, QObject *parent)
    : QAbstractTableModel(parent)
{
    setDataSource(data);
}

void TestInternalVariableModel::setDataSource(const QList<EngineLocalVariable *> &data)
{
    beginResetModel();

    for (auto variable : m_data) {
        disconnect(variable, &EngineLocalVariable::valueChanged, this, &TestInternalVariableModel::dataSourceDataChanged);
    }

    m_data.clear();

    if (!data.isEmpty()) {
        for (auto variable : data) {
            m_data.append(variable);

            connect(variable, &EngineLocalVariable::valueChanged, this, &TestInternalVariableModel::dataSourceDataChanged);
        }
    }
    endResetModel();
}

QVariant TestInternalVariableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    EngineLocalVariable *variable = m_data.at(index.row());
    Q_ASSERT(variable);
    if (!variable)
        return QVariant();

    switch (role) {

        case Qt::DisplayRole: {
            if (index.column() == 0) {
                return variable->name();
            }

            if (index.column() == 1) {
                //return variable->value();
                QVariant value = variable->value();
                //                if (value.canConvert(QVariant::Bool)) {
                //                    return value.toBool() ? "ON" : "OFF";
                //                } else
                if (value.isValid() && !value.isNull()) {
                    QMetaType::Type type = static_cast<QMetaType::Type>(value.type());
                    if (type == QMetaType::Double || type == QMetaType::Float) {
                        return QString::number(variable->value().toReal(), 'g', 5);
                    }
                    return value.toString();
                }
            }
            break;
        }

        case Qt::ToolTipRole: {
            return variable->description();
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

            //        case Qt::BackgroundColorRole: {
            //            if (index.column() == 1 && variable->value().type() == QVariant::Bool)
            //                return variable->value().toBool() == false ? QColor(200, 50, 50) : QColor(50, 150, 50);
            //            break;
            //        }
    }

    return QVariant();
}

int TestInternalVariableModel::rowCount(const QModelIndex &parent) const
{
    return m_data.count();
}

int TestInternalVariableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

Qt::ItemFlags TestInternalVariableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    EngineLocalVariable *variable = static_cast<EngineLocalVariable *>(index.internalPointer());
    Q_ASSERT(variable);
    if (!variable)
        return Qt::ItemIsEnabled;

    if (index.column() == 1 && variable->isWritable()) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return Qt::ItemIsEnabled;
}

QModelIndex TestInternalVariableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, m_data.at(row));
}

bool TestInternalVariableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        EngineLocalVariable *variable = static_cast<EngineLocalVariable *>(index.internalPointer());
        Q_ASSERT(variable);
        if (!variable)
            return false;

        variable->writeValue(value);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant TestInternalVariableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return tr("Имя");
        if (section == 1)
            return tr("Значение");
    }

    return QVariant();
}

void TestInternalVariableModel::dataSourceDataChanged(EngineLocalVariable *variable)
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
