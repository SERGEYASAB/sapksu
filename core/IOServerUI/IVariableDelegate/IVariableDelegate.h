#ifndef IVARIABLEDELEGATE_H
#define IVARIABLEDELEGATE_H

#include "Common/PrefixHeader.h"

#include "Common/FKValidatorSpinBox/FKInt64Validator.h"
#include "Common/FKValidatorSpinBox/FKUInt64Validator.h"

class IVariable;

class IVariableDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit IVariableDelegate(QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

protected:
    void drawItemText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QString &text) const;
    bool showConfirmChangeValueMessage(const QString &valueName) const;
};

#endif // IVARIABLEDELEGATE_H
