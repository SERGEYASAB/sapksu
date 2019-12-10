#ifndef KSU735MANUAL0X20TESTINGVALUEDELEGATE_H
#define KSU735MANUAL0X20TESTINGVALUEDELEGATE_H

#include "Common/PrefixHeader.h"

class Ksu735Manual0x20TestingModel;

class Ksu735Manual0x20TestingValueDelegate : public QStyledItemDelegate
{
public:
    Ksu735Manual0x20TestingValueDelegate(Ksu735Manual0x20TestingModel *model, QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    Ksu735Manual0x20TestingModel *m_model;
};

#endif // KSU735MANUAL0X20TESTINGVALUEDELEGATE_H
