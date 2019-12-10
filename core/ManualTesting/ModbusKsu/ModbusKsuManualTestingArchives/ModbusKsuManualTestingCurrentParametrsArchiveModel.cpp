#include "ModbusKsuManualTestingCurrentParametrsArchiveModel.h"
#include "IOServer/nsNode/ModbusKsuArchive/ModbusKsuCurrentParametrsArchive.h"

ModbusKsuManualTestingCurrentParametrsArchiveModel::ModbusKsuManualTestingCurrentParametrsArchiveModel(ModbusKsuManualTestingArchiveRecordsModel &recordsModel, QObject *parent) :
    QAbstractTableModel(parent),
    m_recordsModel(recordsModel)
{
}

void ModbusKsuManualTestingCurrentParametrsArchiveModel::updateValues()
{
    beginResetModel();
    endResetModel();
}

QVariant ModbusKsuManualTestingCurrentParametrsArchiveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Vertical) {
        if (section == 0)
            return tr("№");
        if (section == 1)
            return tr("Дата");
        if (section == 2)
            return tr("Класс события");
        if (section == 3)
            return tr("Код События");
        if (section == 4)
            return tr("Направление вращения");
        if (section == 5)
            return tr("F, Гц");
        if (section == 6)
            return tr("Ia, А");
        if (section == 7)
            return tr("Ib, А");
        if (section == 8)
            return tr("Ic, А");
        if (section == 9)
            return tr("Дисбаланс I, %");
        if (section == 10)
            return tr("Id, А");
        if (section == 11)
            return tr("Ud, В");
        if (section == 12)
            return tr("Uвых. U, В");
        if (section == 13)
            return tr("Uвых. V, В");
        if (section == 14)
            return tr("Uвых. W, В");
        if (section == 15)
            return tr("Активная мощность, кВт");
        if (section == 16)
            return tr("Реактивная мощность, кВАР");
        if (section == 17)
            return tr("Коэффициент мощности");
        if (section == 18)
            return tr("Загрузка ПЭД, %");
        if (section == 19)
            return tr("Uвх. A, В");
        if (section == 20)
            return tr("Uвх. B, В");
        if (section == 21)
            return tr("Uвх. C, В");
        if (section == 22)
            return tr("Дисбаланс U, %");
        if (section == 23)
            return tr("Сопротивление изоляции, кОм");
        if (section == 24)
            return tr("Давление на приеме насоса, МПа");
        if (section == 25)
            return tr("Температура обмотки ПЭД, °C");
        if (section == 26)
            return tr("Температура жидкости на приеме насоса, °C");
        if (section == 27)
            return tr("Вибрация по оси X+Y, g");
        if (section == 28)
            return tr("Вибрация по оси Z, g");
        if (section == 29)
            return tr("Давление на выкиде насоса, МПа");
        if (section == 30)
            return tr("Температура на выкиде насоса, °C");
        if (section == 31)
            return tr("Расход на выкиде насоса, м³/cут");
        if (section == 32)
            return tr("Дверь");
        if (section == 33)
            return tr("Контактный манометр");
        if (section == 34)
            return tr("Цифровой вход 1");
        if (section == 35)
            return tr("Цифровой вход 2");
        if (section == 36)
            return tr("Цифровой вход 3");
        if (section == 37)
            return tr("Цифровой вход 4");
        if (section == 38)
            return tr("Цифровой вход 5");
        if (section == 39)
            return tr("Цифровой вход 6");
        if (section == 40)
            return tr("Температура в контроллере, °C");
    }

    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Horizontal) {
        if (section < m_recordsModel.records().size()) {
            const ModbusKsuManualTestingArchiveRecord &record = m_recordsModel.records().at(section);
            return tr("Запись ") + QString::number(record.number);
        }
    }

    return QVariant();
}

int ModbusKsuManualTestingCurrentParametrsArchiveModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 41;
}

int ModbusKsuManualTestingCurrentParametrsArchiveModel::columnCount(const QModelIndex &parent) const
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

QVariant ModbusKsuManualTestingCurrentParametrsArchiveModel::data(const QModelIndex &index, int role) const
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
    ModbusKsuCurrentParametrsArchive *archive = qobject_cast<ModbusKsuCurrentParametrsArchive *>(archiveRecord);
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
        return archive->eventClassTranscriptString();
    if (section == 3)
        return archive->eventCodeTranscriptString();
    if (section == 4)
        return archive->rotationDirection();
    if (section == 5)
        return archive->frequency();
    if (section == 6)
        return archive->Ia();
    if (section == 7)
        return archive->Ib();
    if (section == 8)
        return archive->Ic();
    if (section == 9)
        return archive->IImbalance();
    if (section == 10)
        return archive->Id();
    if (section == 11)
        return archive->Ud();
    if (section == 12)
        return archive->UOutputU();
    if (section == 13)
        return archive->UOutputV();
    if (section == 14)
        return archive->UOutputW();
    if (section == 15)
        return archive->activePower();
    if (section == 16)
        return archive->power();
    if (section == 17)
        return archive->cos();
    if (section == 18)
        return archive->load();
    if (section == 19)
        return archive->UInputA();
    if (section == 20)
        return archive->UInputB();
    if (section == 21)
        return archive->UInputC();
    if (section == 22)
        return archive->UImbalance();
    if (section == 23)
        return archive->resistance();
    if (section == 24)
        return archive->pumpInputPressure();
    if (section == 25)
        return archive->windingTemp();
    if (section == 26)
        return archive->pumpAquaInputTemp();
    if (section == 27)
        return archive->XYVibration();
    if (section == 28)
        return archive->ZVibration();
    if (section == 29)
        return archive->pumpOutputPressure();
    if (section == 30)
        return archive->pumpOutputTemp();
    if (section == 31)
        return archive->pumpOutputQ();
    if (section == 32)
        return archive->door();
    if (section == 33)
        return archive->contactManometer();
    if (section == 34)
        return archive->digitalInpit1();
    if (section == 35)
        return archive->digitalInpit2();
    if (section == 36)
        return archive->digitalInpit3();
    if (section == 37)
        return archive->digitalInpit4();
    if (section == 38)
        return archive->digitalInpit5();
    if (section == 39)
        return archive->digitalInpit6();
    if (section == 40)
        return archive->controllerTemp();

    return QVariant();
}
