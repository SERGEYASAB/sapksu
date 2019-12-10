#ifndef PARAMETERALIASSELECTMODEL_H
#define PARAMETERALIASSELECTMODEL_H

#include "Common/PrefixHeader.h"

class ParameterAliasSelectModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    ParameterAliasSelectModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex findItem(const QString &text);
    int columnCount(const QModelIndex &parent) const override;
};

#endif // PARAMETERALIASSELECTMODEL_H
