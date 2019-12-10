#include "Ksu735Manual0x30TestingModel.h"

#include "IOServer/nsNode/Ksu735NodeArchive.h"

Ksu735Manual0x30TestingModel::Ksu735Manual0x30TestingModel(Ksu735Manual0x30TestingArchiveRecordsModel &recordsModel, QObject *parent) :
    QAbstractTableModel(parent),
    m_recordsModel(recordsModel)
{
}

void Ksu735Manual0x30TestingModel::updateValues()
{
    beginResetModel();
    endResetModel();
}

QVariant Ksu735Manual0x30TestingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Horizontal) {
        if (section == 0)
            return tr("№");
        if (section == 1)
            return tr("Дата");
        if (section == 2)
            return tr("Первопричина останова");
        if (section == 3)
            return tr("Характерный параметр");
        if (section == 4)
            return tr("Значение");
    }

    return QVariant();
}

int Ksu735Manual0x30TestingModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (m_recordsModel.hasError()) {
        return 1;
    }

    if (!m_recordsModel.records().isEmpty()) {
        return m_recordsModel.records().size();
    }

    return 0;
}

int Ksu735Manual0x30TestingModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 5;
}

QVariant Ksu735Manual0x30TestingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::ToolTipRole)
        return QVariant();

    if (m_recordsModel.hasError()) {
        if (index.column() == 2) {
            return m_recordsModel.lastError().description();
        }

        return QStringLiteral("---");
    }

    if (index.row() < m_recordsModel.records().size()) {
        const Ksu735Manual0x30TestingArchiveRecord &record = m_recordsModel.records().at(index.row());
        Ksu735NodeArchive *archive = record.archiveRecord;

        if ((!archive || archive->isEmpty()) && index.column() > 0)
            return QStringLiteral("---");

        if (!archive->isValid()) {
            if (index.column() == 0)
                return record.number;
            if (index.column() == 2)
                return QStringLiteral("Неверный формат архива");
            return QStringLiteral("---");
        }

        if (index.column() == 0) {
            return record.number;
        }

        if (index.column() == 1) {
            return archive->recordDate().toString(QStringLiteral("ddd, dd MMM HH:mm"));
        }

        if (index.column() == 2) {
            return archive->stopReasonName();
        }

        if (index.column() == 3) {
            return archive->parametrName();
        }

        if (index.column() == 4) {
            return archive->parametrValue();
        }
    }

    return QVariant();
}
