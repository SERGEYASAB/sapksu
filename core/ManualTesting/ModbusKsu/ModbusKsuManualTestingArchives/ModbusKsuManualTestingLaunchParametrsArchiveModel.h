#ifndef MODBUSKSUMANUALTESTINGLAUNCHPARAMETRSARCHIVEMODEL_H
#define MODBUSKSUMANUALTESTINGLAUNCHPARAMETRSARCHIVEMODEL_H

#include "Common/PrefixHeader.h"
#include "ModbusKsuManualTestingArchiveRecordsModel.h"

class ModbusKsuManualTestingLaunchParametrsArchiveModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ModbusKsuManualTestingLaunchParametrsArchiveModel(ModbusKsuManualTestingArchiveRecordsModel &recordsModel, QObject *parent = 0);

    void updateValues();

    //QAbstractTableModel interface
public:
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    ModbusKsuManualTestingArchiveRecordsModel &m_recordsModel;
};

#endif // MODBUSKSUMANUALTESTINGLAUNCHPARAMETRSARCHIVEMODEL_H
