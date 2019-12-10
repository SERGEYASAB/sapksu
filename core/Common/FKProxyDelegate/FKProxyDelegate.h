#ifndef FKPROXYDELEGATE_H
#define FKPROXYDELEGATE_H

#include "Common/PrefixHeader.h"

class FKProxyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FKProxyDelegate(QObject *parent = Q_NULLPTR);

public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void destroyEditor(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;

    QHash<int, QAbstractItemDelegate *> delegates() const;
    void setDelegates(const QHash<int, QAbstractItemDelegate *> &delegates);

    void setDelegateForColumn(int column, QAbstractItemDelegate *delegate);
    QAbstractItemDelegate *delegateForColumn(int column) const;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;

private:
    QHash<int, QAbstractItemDelegate *> m_delegates;
};

#endif // FKPROXYDELEGATE_H
