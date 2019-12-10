#ifndef KSU0X20PARAMETRSDELEGATE_H
#define KSU0X20PARAMETRSDELEGATE_H

#include "Common/PrefixHeader.h"

class KSU0x20ParametrsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    KSU0x20ParametrsDelegate(QObject *parent = nullptr);

public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
};

#endif // KSU0X20PARAMETRSDELEGATE_H
