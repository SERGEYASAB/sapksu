#ifndef KSU0X20PARAMETRSMODEL_H
#define KSU0X20PARAMETRSMODEL_H

#include "Common/PrefixHeader.h"

class KSU0x20Parametr;

class KSU0x20ParametrsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    KSU0x20ParametrsModel(QVector<KSU0x20Parametr *> &parametrsMap, QObject *parent = nullptr);

//    explicit DeviceModel(QObject *parent = Q_NULLPTR);
//    DeviceModel(const INode *node = Q_NULLPTR, QObject *parent = Q_NULLPTR);

//    void setDataSource(const INode *node);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    inline bool hasChanges() { return m_hasChages; }
    void resetHasChanges() { m_hasChages = false; }

private:
    void fixupParametrValues(KSU0x20Parametr &parametr) const;

    QVector<KSU0x20Parametr *> &m_parametrs;

    bool m_hasChages = false;
};

#endif // KSU0X20PARAMETRSMODEL_H
