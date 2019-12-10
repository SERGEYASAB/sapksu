#include "Ksu735Manual0x10TestingModel.h"

#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

Ksu735Manual0x10TestingModel::Ksu735Manual0x10TestingModel(const VariableValueList &variableValues, QObject *parent)
    : QAbstractTableModel(parent),
      m_variableValues(variableValues)
{
}

void Ksu735Manual0x10TestingModel::updateValues()
{
    beginResetModel();
    emit dataChanged(index(0, 2), index(rowCount(), 2));
    endResetModel();
}

QVariant Ksu735Manual0x10TestingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Horizontal) {
        if (section == 0)
            return tr("Маска");
        if (section == 1)
            return tr("Описание");
        if (section == 2)
            return tr("Полученное значение");
    }

    return QVariant();
}

int Ksu735Manual0x10TestingModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_variableValues.length();
}

int Ksu735Manual0x10TestingModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 3;
}

QVariant Ksu735Manual0x10TestingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Ksu735Variable *variable = m_variableValues.at(index.row())->variable;
    Q_ASSERT(variable);

    switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            if (index.column() == 0) {
                quint32 address = variable->address();
                if (address == 0) {
                    return QStringLiteral("---");
                } else {
                    QString text;
                    QTextStream stream(&text);
                    stream.setNumberFlags(QTextStream::UppercaseDigits);

                    stream << left << "0x";

                    stream.setFieldWidth(8);
                    stream.setPadChar('0');

                    stream << right << hex << address;
                    stream.flush();

                    return text;
                }
            }

            if (index.column() == 1) {
                return variable->name();
            }

            if (index.column() == 2) {
                QVariant &value = m_variableValues.at(index.row())->value;
                if (!value.isValid()) {
                    return QStringLiteral("---");
                }
                return value;
            }

            break;
        }

        case Qt::ForegroundRole: {
            return QVariant(QBrush(Qt::black));
            break;
        }

        case Qt::TextAlignmentRole: {
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
            break;
        }

        case Qt::SizeHintRole: {
            if (index.column() == 0)
                return QSize(21, 40);
            if (index.column() == 1)
                return QSize(21, 100);
            if (index.column() == 2)
                return QSize(21, 300);
            break;
        }
    }

    return QVariant();
}

QModelIndex Ksu735Manual0x10TestingModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, m_variableValues.at(row)->variable);
}
