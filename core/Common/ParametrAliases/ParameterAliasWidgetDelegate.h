#ifndef PARAMETERALIASWIDGETDELEGATE_H
#define PARAMETERALIASWIDGETDELEGATE_H

#include "Common/PrefixHeader.h"

class ParameterAliasWidgetDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ParameterAliasWidgetDelegate(QObject *parent = Q_NULLPTR);

    //  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // PARAMETERALIASWIDGETDELEGATE_H
