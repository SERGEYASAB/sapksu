#ifndef MODBUSKSUMANUALSELECTIVETESTINGMODEL_H
#define MODBUSKSUMANUALSELECTIVETESTINGMODEL_H

#include "Common/PrefixHeader.h"
#include "ModbusKsuManualSelectiveTestingParametr.h"
#include "ManualTesting/Common/IVariableByIndex.h"

class ModbusKsuNode;

class ModbusKsuManualSelectiveTestingModel : public QAbstractTableModel, public IVariableByIndex
{
    Q_OBJECT

public:
    explicit ModbusKsuManualSelectiveTestingModel(ModbusKsuNode *node,
                                                  ModbusKsuManualSelectiveTestingParametrsList &parametrsList,
                                                  QObject *parent = 0);

    inline ModbusKsuManualSelectiveTestingParametrsList &parametrsList() const;
    void setParametrsList(ModbusKsuManualSelectiveTestingParametrsList &parametrsList);

    ModbusKsuManualSelectiveTestingParametr *parametrByIndex(const QModelIndex &index) const;

    void updateValues();
    void updateParametr(ModbusKsuManualSelectiveTestingParametr *parametr);

    //IVariableByIndex interface
public:
    IVariable *variableByIndex(const QModelIndex &index) const override;

    //QAbstractTableModel interface
public:
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    ModbusKsuManualSelectiveTestingParametr *parametrByIndex(int index) const;

private:
    ModbusKsuNode *m_node = nullptr;
    ModbusKsuManualSelectiveTestingParametrsList &m_parametrsList;
};

inline ModbusKsuManualSelectiveTestingParametrsList &ModbusKsuManualSelectiveTestingModel::parametrsList() const
{
    return m_parametrsList;
}

#endif // MODBUSKSUMANUALSELECTIVETESTINGMODEL_H
