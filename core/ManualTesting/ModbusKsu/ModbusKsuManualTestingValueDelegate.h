#ifndef MODBUSKSUMANUALTESTINGVALUEDELEGATE_H
#define MODBUSKSUMANUALTESTINGVALUEDELEGATE_H

#include "Common/PrefixHeader.h"
#include "ManualTesting/Common/IVariableByIndex.h"

class ModbusKsuManualTestingValueDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ModbusKsuManualTestingValueDelegate(IVariableByIndex *model, QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
//    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

protected:
    void drawItemText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QString &text) const;

private:
    IVariableByIndex *m_model = nullptr;
};

#endif // MODBUSKSUMANUALTESTINGVALUEDELEGATE_H
