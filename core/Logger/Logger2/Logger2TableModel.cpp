#include "Logger2TableModel.h"

Logger2TableModel::Logger2TableModel()
{

}

void Logger2TableModel::append(const QColor &color, const QString &text)
{

}

QVariant Logger2TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {

        case Qt::DisplayRole: {
            if (index.column() == 0) {
                return m_logLines.at(index.row()).text;
            }
            break;
        }

        case Qt::ToolTipRole: {
            break;
        }

        case Qt::ForegroundRole: {
                return QVariant(m_logLines.at(index.row()).color);
            break;
        }

        case Qt::TextAlignmentRole: {
            return Qt::AlignLeft;
            break;
        }
    }

    return QVariant();
}

int Logger2TableModel::rowCount(const QModelIndex &parent) const
{
    return m_logLines.count();
}

int Logger2TableModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}
