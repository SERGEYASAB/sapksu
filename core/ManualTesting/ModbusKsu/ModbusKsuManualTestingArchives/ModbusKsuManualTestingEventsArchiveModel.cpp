#include "ModbusKsuManualTestingEventsArchiveModel.h"
#include "IOServer/nsNode/ModbusKsuArchive/ModbusKsuEventsArchive.h"

ModbusKsuManualTestingEventsArchiveModel::ModbusKsuManualTestingEventsArchiveModel(ModbusKsuManualTestingArchiveRecordsModel &recordsModel, QObject *parent) :
    QAbstractTableModel(parent),
    m_recordsModel(recordsModel)
{

}

void ModbusKsuManualTestingEventsArchiveModel::updateValues()
{
    beginResetModel();
    endResetModel();
}

QVariant ModbusKsuManualTestingEventsArchiveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Horizontal) {
        if (section == 0)
            return tr("№");
        if (section == 1)
            return tr("Дата");
        if (section == 2)
            return tr("Класс события");
        if (section == 3)
            return tr("Код События");
        if (section == 4)
            return tr("Вид запуска / Причина останова");
        if (section == 5)
            return tr("Параметр (причины останова)");
        if (section == 6)
            return tr("Значение параметра");
    }

    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Vertical) {
        if (section < m_recordsModel.records().size()) {
            const ModbusKsuManualTestingArchiveRecord &record = m_recordsModel.records().at(section);
            return record.number;
        }
    }

    return QVariant();
}

int ModbusKsuManualTestingEventsArchiveModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    int rowCount = 0;

    if (m_recordsModel.hasError()) {
        rowCount++;
    }

    if (!m_recordsModel.records().isEmpty()) {
        rowCount += m_recordsModel.records().size();
    }

    return rowCount;
}

int ModbusKsuManualTestingEventsArchiveModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 7;
}

QVariant ModbusKsuManualTestingEventsArchiveModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::ToolTipRole)
        return QVariant();

    if (m_recordsModel.hasError() && index.row() == rowCount() - 1) {
        if (index.column() == 1) {
            return m_recordsModel.lastError().description();
        }

        return QStringLiteral("---");
    }

    if (index.row() >= m_recordsModel.records().size())
        return QVariant();

    const ModbusKsuManualTestingArchiveRecord &record = m_recordsModel.records().at(index.row());
    IModbusKsuArchive *archiveRecord = record.archiveRecord;
    ModbusKsuEventsArchive *archive = qobject_cast<ModbusKsuEventsArchive *>(archiveRecord);
    if (!archive) {
        if (index.column() == 1) {
            return QStringLiteral("Неправильный формат архивной записи");
        }

        return QStringLiteral("---");
    }

    if (archive->isEmpty()) {
        if (index.column() == 1) {
            return QStringLiteral("Запись пуста");
        }

        return QStringLiteral("---");
    }

    if (index.column() == 0) {
        return archive->number();
    }

    if (index.column() == 1) {
        return archive->date().toString(QStringLiteral("ddd, HH:mm:ss dd/MM/yyyy"));
    }

    if (index.column() == 2) {
        return archive->eventClassTranscriptString();
    }

    if (index.column() == 3) {
        return archive->eventCodeTranscriptString();
    }

    if (index.column() == 4) {
        return archive->kindOfLaunchOrStopReasonString();
    }

    if (index.column() == 5) {
        return archive->stopParametrName();
    }

    if (index.column() == 6) {
        return archive->stopParametrValue();
    }

    return QVariant();
}
