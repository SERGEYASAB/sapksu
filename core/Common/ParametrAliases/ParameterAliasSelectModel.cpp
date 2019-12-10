#include "ParameterAliasSelectModel.h"

ParameterAliasSelectModel::ParameterAliasSelectModel(QObject *parent) :
    QSqlQueryModel(parent)
{

}

QVariant ParameterAliasSelectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    query().seek(index.row());
    switch (role) {
        case Qt::DisplayRole: {

            switch (index.column()) {
                case 0: {
                    return query().record().value("alias");
                }
                case 1: {
                    return query().record().value("description");
                }
            }
            break;
        }

        default: {
            return QSqlQueryModel::data(index, role);
        }
    }

    return QVariant();
}

QModelIndex ParameterAliasSelectModel::findItem(const QString &text)
{
    QModelIndexList matches = match(index(0, 0), Qt::DisplayRole, text);

    if (!matches.isEmpty())
        return matches.first();
    else
        return QModelIndex();
}

int ParameterAliasSelectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}
