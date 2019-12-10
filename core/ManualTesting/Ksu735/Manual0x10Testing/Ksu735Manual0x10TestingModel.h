#ifndef KSU735MANUAL0X10TESTINGMODEL_H
#define KSU735MANUAL0X10TESTINGMODEL_H

#include "Common/PrefixHeader.h"

class Ksu735Node;
class Ksu735Variable;

class Ksu735Manual0x10TestingModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    struct VariableValue {
        Ksu735Variable *variable;
        QVariant value;
    };
    typedef QList<VariableValue *> VariableValueList;

    explicit Ksu735Manual0x10TestingModel(const VariableValueList &variableValues, QObject *parent = 0);

    void updateValues();

    //QAbstractTableModel interface
public:
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

private:
    VariableValueList m_variableValues;
};

#endif // KSU735MANUAL0X10TESTINGMODEL_H
