#ifndef KSU735MANUALFULLTESTINGVALUEDELEGATE_H
#define KSU735MANUALFULLTESTINGVALUEDELEGATE_H

#include "Common/PrefixHeader.h"

class Ksu735ManualFullTestingModel;

class Ksu735ManualFullTestingValueDelegate : public QStyledItemDelegate
{
public:
    Ksu735ManualFullTestingValueDelegate(Ksu735ManualFullTestingModel *model, QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    Ksu735ManualFullTestingModel *m_model;
};

#endif // KSU735MANUALFULLTESTINGVALUEDELEGATE_H
