#ifndef KSU735MANUAL0X20TESTINGMODEL_H
#define KSU735MANUAL0X20TESTINGMODEL_H

#include "Common/PrefixHeader.h"
#include "Ksu735Manual0x20TestingParametr.h"

class Ksu735Node;

class Ksu735Manual0x20TestingModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit Ksu735Manual0x20TestingModel(Ksu735Node *node,
                                          Ksu735Manual0x20TestingParametrsList &parametrsList,
                                          QObject *parent = nullptr);

    inline Ksu735Manual0x20TestingParametrsList &parametrsList() const;
    void setParametrsList(Ksu735Manual0x20TestingParametrsList &parametrsList);

    Ksu735Manual0x20TestingParametr *parametrByIndex(const QModelIndex &index) const;

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

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    Ksu735Node *m_node = nullptr;
    Ksu735Manual0x20TestingParametrsList &m_parametrsList;
};

inline Ksu735Manual0x20TestingParametrsList &Ksu735Manual0x20TestingModel::parametrsList() const
{
    return m_parametrsList;
}

#endif // KSU735MANUAL0X20TESTINGMODEL_H
