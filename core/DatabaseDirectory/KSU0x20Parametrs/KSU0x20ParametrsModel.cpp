#include "KSU0x20ParametrsModel.h"
#include "KSU0x20Parametr.h"

KSU0x20ParametrsModel::KSU0x20ParametrsModel(QVector<KSU0x20Parametr *> &parametrsMap, QObject *parent) :
    QAbstractTableModel(parent), m_parametrs(parametrsMap)
{

}

QVariant KSU0x20ParametrsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    KSU0x20Parametr *parametr = m_parametrs[index.row()];
    if (!parametr) {
        if (role == Qt::DisplayRole && index.column() == 1) {
            return index.row();
        }

        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole: {
            if (index.column() == 1)
                return parametr->address();
            if (index.column() == 2) {
                if (!parametr->enabled() && parametr->name().isEmpty())
                    return tr("Зарезервирован");
                return parametr->name();
            }
            if (index.column() == 3)
                return parametr->alias();
            if (index.column() == 4)
                return parametr->format();
            if (index.column() == 5)
                return parametr->writable();
            if (index.column() == 6) {
                if (parametr->format() == SapKsu::CCCC)
                    return QVariant();
                return parametr->minValue();
            }
            if (index.column() == 7) {
                if (parametr->format() == SapKsu::CCCC)
                    return QVariant();
                return parametr->maxValue();
            }
            if (index.column() == 8) {
                if (parametr->format() == SapKsu::CCCC)
                    return QVariant();
                return parametr->defaultValue();
            }
            if (index.column() == 9)
                return parametr->permissibleVariation();

            if (index.column() == 10)
                return parametr->measureUnit();

            break;
        }

        case Qt::ToolTipRole: {
            if (index.column() == 2)
                return parametr->name();
            break;
        }

        case Qt::TextAlignmentRole: {
            return int(Qt::AlignLeft | Qt::AlignVCenter);
            break;
        }

        case Qt::CheckStateRole: {
            if (index.column() == 0) {
                if (parametr->enabled())
                    return Qt::CheckState::Checked;
                return Qt::CheckState::Unchecked;
            }
        }
    }

    return QVariant();
}

int KSU0x20ParametrsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_parametrs.count();
}

int KSU0x20ParametrsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 11;
}

Qt::ItemFlags KSU0x20ParametrsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.column() == 0) {
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
    } else if (index.column() == 1) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
    } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemNeverHasChildren;
    }
}

QModelIndex KSU0x20ParametrsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, m_parametrs.at(row));
}

bool KSU0x20ParametrsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    KSU0x20Parametr *parametr = static_cast<KSU0x20Parametr *>(index.internalPointer());
    bool hasParametr = true;
    if (!parametr) {
        hasParametr = false;
        parametr = new KSU0x20Parametr(QObject::parent());
        parametr->setAddress(index.row());
        m_parametrs[index.row()] = parametr;
    }

    if (index.column() == 0) {
        if (role == Qt::CheckStateRole) {
            bool data = value.toBool();
            parametr->setEnabled(data);
            dataChanged(index, index, QVector<int>() << role);
            return true;
        }
    } else {
        if (index.column() == 2) {
            parametr->setName(value.toString().trimmed());
            parametr->setEnabled(!value.toString().trimmed().isEmpty());
        } else if (index.column() == 3) {
            parametr->setAlias(value.toString());
        } else if (index.column() == 4) {
            QMetaEnum formatEnum = QMetaEnum::fromType<SapKsu::Ksu735ParametrFormat>();
            bool ok = true;
            SapKsu::Ksu735ParametrFormat format = SapKsu::Ksu735ParametrFormat(formatEnum.value(value.toInt(&ok)));
            if (ok && format != -1) {
                parametr->setFormat(format);
                fixupParametrValues(*parametr);
            }
        } else if (index.column() == 5) {
            parametr->setWritable(value.toBool());
        } else if (index.column() == 6 || index.column() == 7 || index.column() == 8) {
            bool ok = true;
            quint16 uintValue = value.toUInt(&ok);
            if (ok) {
                if (index.column() == 6) parametr->setMinValue(uintValue);
                if (index.column() == 7) parametr->setMaxValue(uintValue);
                if (index.column() == 8) parametr->setDefaultValue(uintValue);

                fixupParametrValues(*parametr);
            }
        } else if (index.column() == 9) {
            parametr->setPermissibleVariation(value.toString());
        } else if (index.column() == 10) {
            parametr->setMeasureUnit(value.toString());
        }

        if (hasParametr)
            dataChanged(index, index, QVector<int>() << role);
        else
            dataChanged(this->index(index.row(), 0, QModelIndex()),
                        this->index(index.row(), this->columnCount() - 1, QModelIndex()));

        m_hasChages = true;
        return true;
    }

    return false;
}

QVariant KSU0x20ParametrsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section == 1)
            return tr("Код");
        if (section == 2)
            return tr("Параметр");
        if (section == 3)
            return tr("Алиас");
        if (section == 4)
            return tr("Формат");
        if (section == 5)
            return tr("Доступ");
        if (section == 6)
            return tr("Мин");
        if (section == 7)
            return tr("Макс");
        if (section == 8)
            return tr("Умолч.");
        if (section == 9)
            return tr("Допустимое отклонение");
        if (section == 10)
            return tr("Ед. Изм.");

    }
    return QVariant();
}

void KSU0x20ParametrsModel::fixupParametrValues(KSU0x20Parametr &parametr) const
{
    SapKsu::Ksu735ParametrFormat format = parametr.format();
    if (format < SapKsu::Ksu735ParametrFormat::XX1XX) {
        parametr.setMinValue(qMin(quint16(9999), parametr.minValue()));
        parametr.setMaxValue(qMin(quint16(9999), parametr.maxValue()));
        parametr.setDefaultValue(qMin(quint16(9999), parametr.defaultValue()));
    }
}
