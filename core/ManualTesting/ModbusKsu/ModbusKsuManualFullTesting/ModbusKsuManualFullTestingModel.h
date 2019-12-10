#ifndef MODBUSKSUMANUALFULLTESTINGMODEL_H
#define MODBUSKSUMANUALFULLTESTINGMODEL_H

#include "Common/PrefixHeader.h"
#include "ManualTesting/ModbusKsu/ModbusKsuManualTestingParametr.h"
#include "ManualTesting/Common/IVariableByIndex.h"

class ModbusKsuNode;

class ModbusKsuManualFullTestingModel : public QAbstractTableModel, public IVariableByIndex
{
    Q_OBJECT

public:
    explicit ModbusKsuManualFullTestingModel(ModbusKsuNode *node,
                                             QList<ModbusKsuManualTestingParametr *> &parametrsList,
                                             QObject *parent = 0);

    void updateAll();
    void updateEnabledParametrsList();
    void updateColumn(int column);
    void updateParametr(ModbusKsuManualTestingParametr *parametr);

    bool showEnabledOnly() const;
    void setShowEnabledOnly(bool showEnabledOnly);

    ModbusKsuManualTestingParametr *parametrByIndex(const QModelIndex &index) const;

    inline QColor okColor() const { return m_okColor; }
    inline QColor notSameColor() const { return m_notSameColor; }
    inline QColor errorColor() const { return m_errorColor; }

    void setOkColor(const QColor &color);
    void setNotSameColor(const QColor &color);
    void setErrorColor(const QColor &color);

    /// IVariableByIndex interface
public:
    IVariable *variableByIndex(const QModelIndex &index) const override;

    /// QAbstractTableModel interface
public:
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    void updateBackgound();

private:
    ModbusKsuNode *m_node = nullptr;
    QList<ModbusKsuManualTestingParametr *> &m_parametrsList;
    QList<ModbusKsuManualTestingParametr *> m_enabledParametrsList;

    bool m_showEnabledOnly = false;

    QColor m_okColor = Qt::green;
    QColor m_notSameColor = Qt::yellow;
    QColor m_errorColor = Qt::red;
};

#endif // MODBUSKSUMANUALFULLTESTINGMODEL_H
