#ifndef KSUMODBUSPARAMETRSMODEL_H
#define KSUMODBUSPARAMETRSMODEL_H

#include "Common/PrefixHeader.h"

class KSUModbusParamertDomItem;

class KSUModbusParametrsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    KSUModbusParametrsModel(QList <KSUModbusParamertDomItem *> parametrs, QObject *parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    inline bool hasChanges() { return m_hasChages; }

public slots:
    void removeRow(int row);
    void appendParametr(KSUModbusParamertDomItem *parametrItem);
    void parametrItemUpdated(KSUModbusParamertDomItem *parametrItem);

private:
    QList<KSUModbusParamertDomItem *> m_parametrs;

    bool m_hasChages = false;
};

#endif // KSUMODBUSPARAMETRSMODEL_H
