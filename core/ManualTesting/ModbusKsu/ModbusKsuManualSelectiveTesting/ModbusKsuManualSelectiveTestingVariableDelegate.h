#ifndef MODBUSKSUMANUALSELECTIVETESTINGVARIABLEDELEGATE_H
#define MODBUSKSUMANUALSELECTIVETESTINGVARIABLEDELEGATE_H

#include "Common/PrefixHeader.h"

class ModbusKsuNode;
class ModbusKsuManualSelectiveTestingModel;

class ModbusKsuManualSelectiveTestingVariableDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ModbusKsuManualSelectiveTestingVariableDelegate(ModbusKsuNode *node,
                                                    ModbusKsuManualSelectiveTestingModel *model,
                                                    QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    ModbusKsuNode *m_node = nullptr;
    ModbusKsuManualSelectiveTestingModel *m_model;

    mutable int initialVariableAddress = -1;
    mutable QModelIndex initialEditorIndex = QModelIndex();
};

#endif // MODBUSKSUMANUALSELECTIVETESTINGVARIABLEDELEGATE_H
