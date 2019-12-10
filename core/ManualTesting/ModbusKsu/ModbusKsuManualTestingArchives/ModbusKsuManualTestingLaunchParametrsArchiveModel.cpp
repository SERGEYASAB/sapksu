#include "ModbusKsuManualTestingLaunchParametrsArchiveModel.h"
#include "IOServer/nsNode/ModbusKsuArchive/ModbusKsuLaunchParametrsArchive.h"

ModbusKsuManualTestingLaunchParametrsArchiveModel::ModbusKsuManualTestingLaunchParametrsArchiveModel(ModbusKsuManualTestingArchiveRecordsModel &recordsModel, QObject *parent) :
    QAbstractTableModel(parent),
    m_recordsModel(recordsModel)
{
}

void ModbusKsuManualTestingLaunchParametrsArchiveModel::updateValues()
{
    beginResetModel();
    endResetModel();
}

QVariant ModbusKsuManualTestingLaunchParametrsArchiveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Vertical) {
        if (section == 0)
            return tr("№");
        if (section == 1)
            return tr("Дата");
        if (section == 2)
            return tr("Миллисекунды");
        if (section == 3)
            return tr("Класс события");
        if (section == 4)
            return tr("Код События");
        if (section == 5)
            return tr("Ia, А");
        if (section == 6)
            return tr("Ib, А");
        if (section == 7)
            return tr("Ic, А");
        if (section == 8)
            return tr("Дисбаланс I, %");
        if (section == 9)
            return tr("Uвх. A, В");
        if (section == 10)
            return tr("Uвх. B, В");
        if (section == 11)
            return tr("Uвх. C, В");
        if (section == 12)
            return tr("Дисбаланс U, %");
        if (section == 13)
            return tr("Сопротивление изоляции, кОм");
        if (section == 14)
            return tr("Загрузка ПЭД, %");
        if (section == 15)
            return tr("Коэффициент мощности");
    }

    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Horizontal) {
        if (section < m_recordsModel.records().size()) {
            const ModbusKsuManualTestingArchiveRecord &record = m_recordsModel.records().at(section);
            return tr("Запись ") + QString::number(record.number);
        }
    }

    return QVariant();
}

int ModbusKsuManualTestingLaunchParametrsArchiveModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 16;
}

int ModbusKsuManualTestingLaunchParametrsArchiveModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    int count = 0;

    if (m_recordsModel.hasError()) {
        count++;
    }

    if (!m_recordsModel.records().isEmpty()) {
        count += m_recordsModel.records().size();
    }

    return count;
}

QVariant ModbusKsuManualTestingLaunchParametrsArchiveModel::data(const QModelIndex &index, int role) const
{
    int section = index.row();

    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::ToolTipRole)
        return QVariant();

    if (m_recordsModel.hasError() && index.column() == columnCount() - 1) {
        if (section == 1) {
            return m_recordsModel.lastError().description();
        }

        return QStringLiteral("---");
    }

    if (index.column() >= m_recordsModel.records().size())
        return QVariant();

    const ModbusKsuManualTestingArchiveRecord &record = m_recordsModel.records().at(index.column());
    IModbusKsuArchive *archiveRecord = record.archiveRecord;
    ModbusKsuLaunchParametrsArchive *archive = qobject_cast<ModbusKsuLaunchParametrsArchive *>(archiveRecord);
    if (!archive) {
        if (section == 1) {
            return QStringLiteral("Неправильный формат архивной записи");
        }

        return QStringLiteral("---");
    }

    if (archive->isEmpty()) {
        if (section == 1) {
            return QStringLiteral("Запись пуста");
        }

        return QStringLiteral("---");
    }

    if (section == 0)
        return archive->number();
    if (section == 1)
        return archive->date().toString(QStringLiteral("ddd, HH:mm:ss dd/MM/yyyy"));
    if (section == 2)
        return archive->milliseconds();
    if (section == 3)
        return archive->eventClassTranscriptString();
    if (section == 4)
        return archive->eventCodeTranscriptString();
    if (section == 5)
        return archive->Ia();
    if (section == 6)
        return archive->Ib();
    if (section == 7)
        return archive->Ib();
    if (section == 8)
        return archive->IImbalance();
    if (section == 9)
        return archive->UInputA();
    if (section == 10)
        return archive->UInputB();
    if (section == 11)
        return archive->UInputC();
    if (section == 12)
        return archive->UImbalance();
    if (section == 13)
        return archive->resistance();
    if (section == 14)
        return archive->load();
    if (section == 15)
        return archive->cos();

    return QVariant();
}
