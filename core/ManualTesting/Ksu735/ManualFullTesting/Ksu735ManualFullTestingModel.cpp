#include "Ksu735ManualFullTestingModel.h"

#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

Ksu735ManualFullTestingModel::Ksu735ManualFullTestingModel(Ksu735Node *node, Ksu735ManualFullTestingParametrsList &parametrsList, QObject *parent) :
    QAbstractTableModel(parent),
    m_node(node),
    m_parametrsList(parametrsList)
{
    for (auto parametr : m_parametrsList) {
        if (parametr->enabled) {
            m_enabledParametrsList.append(parametr);
        }
    }
}

void Ksu735ManualFullTestingModel::updateAll()
{
    beginResetModel();
    endResetModel();
}

void Ksu735ManualFullTestingModel::updateEnabledParametrsList()
{
    if (m_showEnabledOnly) {
        beginResetModel();
    }

    m_enabledParametrsList.clear();
    for (auto parametr : m_parametrsList) {
        if (parametr->enabled) {
            m_enabledParametrsList.append(parametr);
        }
    }

    if (m_showEnabledOnly) {
        endResetModel();
    } else {
        emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
    }
}

void Ksu735ManualFullTestingModel::updateColumn(int column)
{
    if (column >= columnCount())
        return;

    emit dataChanged(index(0, column), index(rowCount() - 1, column));
}

void Ksu735ManualFullTestingModel::updateParametr(Ksu735ManualFullTestingParametr *parametr)
{
    int row = -1;
    if (m_showEnabledOnly) {
        row = m_enabledParametrsList.indexOf(parametr);
    } else {
        row = m_parametrsList.indexOf(parametr);
    }

    if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

bool Ksu735ManualFullTestingModel::showEnabledOnly() const
{
    return m_showEnabledOnly;
}

void Ksu735ManualFullTestingModel::setShowEnabledOnly(bool showEnabledOnly)
{
    if (m_showEnabledOnly != showEnabledOnly) {
        beginResetModel();
        m_showEnabledOnly = showEnabledOnly;
        endResetModel();
    }
}

Ksu735ManualFullTestingParametr *Ksu735ManualFullTestingModel::parametrByIndex(const QModelIndex &index) const
{
    if (m_showEnabledOnly && index.row() >= 0 && index.row() < m_enabledParametrsList.count()) {

    } else if (!m_showEnabledOnly && index.row() >= 0 && index.row() < m_parametrsList.count()) {
        return m_parametrsList.at(index.row());
    }
    return nullptr;
}

void Ksu735ManualFullTestingModel::setOkColor(const QColor &color)
{
    if (m_okColor != color) {
        m_okColor = color;
        updateBackgound();
    }
}

void Ksu735ManualFullTestingModel::setNotSameColor(const QColor &color)
{
    if (m_notSameColor != color) {
        m_notSameColor = color;
        updateBackgound();
    }
}

void Ksu735ManualFullTestingModel::setErrorColor(const QColor &color)
{
    if (m_errorColor != color) {
        m_errorColor = color;
        updateBackgound();
    }
}

QVariant Ksu735ManualFullTestingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Horizontal) {
        if (section == 1)
            return tr("Код");
        if (section == 2)
            return tr("Параметр");
        if (section == 3)
            return tr("Формат");
        if (section == 4)
            return tr("Доступ");
        if (section == 5)
            return tr("Мин");
        if (section == 6)
            return tr("Макс");
        if (section == 7)
            return tr("Умолчание");
        if (section == 8)
            return tr("Допустимое отклонение");
        if (section == 9)
            return tr("Настройка");
        if (section == 10)
            return tr("Прошлая настройка");
        if (section == 11)
            return tr("Нов");
        if (section == 12)
            return tr("Обн");
        if (section == 13)
            return tr("Статус");
    }

    return QVariant();
}

int Ksu735ManualFullTestingModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (m_showEnabledOnly)
        return m_enabledParametrsList.size();
    return m_parametrsList.size();
}

int Ksu735ManualFullTestingModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 14;
}

QVariant Ksu735ManualFullTestingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

    Ksu735ManualFullTestingParametr *parametr;
    if (m_showEnabledOnly) {
        parametr = m_enabledParametrsList.at(index.row());
    } else {
        parametr = m_parametrsList.at(index.row());
    }

    if (index.column() == 0 && role == Qt::CheckStateRole) {
        if (parametr->enabled)
            return Qt::CheckState::Checked;
        return Qt::CheckState::Unchecked;
    }

    switch (role) {
        case Qt::DisplayRole: {
            if (index.column() == 1) {
                return parametr->variable()->address();
            }

            if (index.column() == 2) {
                return parametr->variable()->name();
            }

            if (index.column() == 3) {
                return parametr->variable()->format();
            }

            if (index.column() == 4) {
                return parametr->variable()->isWritable();
            }

            if (index.column() == 5) {
                return parametr->variable()->minValue();
            }

            if (index.column() == 6) {
                return parametr->variable()->maxValue();
            }

            if (index.column() == 7) {
                return parametr->variable()->defaultValue();
            }

            if (index.column() == 8) {
                return parametr->variable()->permissibleVariation();
            }

            if (index.column() == 9) {
                return parametr->writingValue;
            }

            if (index.column() == 10){
                return parametr->oldReadedValue;
            }

            if (index.column() == 11){
                return parametr->writedValue;
            }

            if (index.column() == 12) {
                return parametr->readedValue;
            }

            if (index.column() == 13) {
                if (parametr->status == SapKsu::FullTestingStatus::Ok)
                    return QStringLiteral("Ок");
                else if (parametr->status == SapKsu::FullTestingStatus::NotSame)
                    return QStringLiteral("Значения не равны");
                else if (parametr->status == SapKsu::FullTestingStatus::Error)
                    return parametr->statusString;
                return QVariant();
            }

            break;
        }

        case Qt::ToolTipRole: {
            if (index.column() == 2) {
                return parametr->variable()->name();
            }

            if (index.column() == 13) {
                return data(index, Qt::DisplayRole);
            }

            break;
        }

        case Qt::EditRole: {
            if (index.column() == 3) {
                return parametr->variable()->format();
            }

            if (index.column() == 4) {
                return parametr->variable()->isWritable();
            }

            if (index.column() == 9) {
                return parametr->writingValue;
            }

            break;
        }

        case Qt::BackgroundRole: {
            if (parametr->status == SapKsu::FullTestingStatus::Ok)
                return m_okColor;
            else if (parametr->status == SapKsu::FullTestingStatus::NotSame)
                return m_notSameColor;
            else if (parametr->status == SapKsu::FullTestingStatus::Error)
                return m_errorColor;
            return QVariant();
        }
    }

    return QVariant();
}

QModelIndex Ksu735ManualFullTestingModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (m_showEnabledOnly) {
        return createIndex(row, column, m_enabledParametrsList.at(row));
    } else {
        return createIndex(row, column, m_parametrsList.at(row));
    }
}

bool Ksu735ManualFullTestingModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    Ksu735ManualFullTestingParametr *parametr = nullptr;
    if (m_showEnabledOnly) {
        parametr = m_enabledParametrsList.at(idx.row());
    } else {
        parametr = m_parametrsList.at(idx.row());
    }

    if (idx.column() == 0 && role == Qt::CheckStateRole) {
        bool data = value.toBool();

        parametr->enabled = data;
        emit dataChanged(idx, idx, QVector<int>() << role);

        updateEnabledParametrsList();
        return true;
    } else if (idx.column() == 9 && role == Qt::EditRole) {
        parametr->writingValue = value;
        emit dataChanged(idx, idx);
    }

    return false;
}

Qt::ItemFlags Ksu735ManualFullTestingModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.column() == 0 || index.column() == 9) {
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemNeverHasChildren;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

void Ksu735ManualFullTestingModel::updateBackgound()
{
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1), QVector<int>() << Qt::BackgroundRole);
}
