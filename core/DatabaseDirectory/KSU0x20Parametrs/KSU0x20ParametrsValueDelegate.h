#ifndef KSU0X20PARAMETRSVALUEDELEGATE_H
#define KSU0X20PARAMETRSVALUEDELEGATE_H

#include "Common/PrefixHeader.h"
#include "KSU0x20Parametr.h"

class KSU0x20ParametrsValueDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    KSU0x20ParametrsValueDelegate(QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};

#endif // KSU0X20PARAMETRSVALUEDELEGATE_H
