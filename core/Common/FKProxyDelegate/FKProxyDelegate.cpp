#include "FKProxyDelegate.h"

FKProxyDelegate::FKProxyDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void FKProxyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (m_delegates.contains(index.column())) {
        m_delegates.value(index.column())->paint(painter, option, index);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize FKProxyDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (m_delegates.contains(index.column()))
        return m_delegates.value(index.column())->sizeHint(option, index);

    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *FKProxyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (m_delegates.contains(index.column()))
        return m_delegates.value(index.column())->createEditor(parent, option, index);

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void FKProxyDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
    if (m_delegates.contains(index.column())) {
        m_delegates.value(index.column())->destroyEditor(editor, index);
    } else {
        QStyledItemDelegate::destroyEditor(editor, index);
    }
}

void FKProxyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (m_delegates.contains(index.column())) {
        m_delegates.value(index.column())->setEditorData(editor, index);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void FKProxyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (m_delegates.contains(index.column())) {
        m_delegates.value(index.column())->setModelData(editor, model, index);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void FKProxyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (m_delegates.contains(index.column())) {
        m_delegates.value(index.column())->updateEditorGeometry(editor, option, index);
    } else {
        QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    }
}

bool FKProxyDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (m_delegates.contains(index.column()))
        return m_delegates.value(index.column())->editorEvent(event, model, option, index);

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

bool FKProxyDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (m_delegates.contains(index.column()))
        return m_delegates.value(index.column())->helpEvent(event, view, option, index);

    return QStyledItemDelegate::helpEvent(event, view, option, index);
}

QHash<int, QAbstractItemDelegate *> FKProxyDelegate::delegates() const
{
    return m_delegates;
}

void FKProxyDelegate::setDelegates(const QHash<int, QAbstractItemDelegate *> &delegates)
{
    auto begin = delegates.cbegin();
    while (begin != delegates.cend()) {
        setDelegateForColumn(begin.key(), begin.value());
        begin++;
    }
}

void FKProxyDelegate::setDelegateForColumn(int column, QAbstractItemDelegate *delegate)
{
    if (m_delegates.contains(column)) {
        QAbstractItemDelegate *oldDelegate = m_delegates.take(column);
        if (oldDelegate) {
            disconnect(oldDelegate, &QAbstractItemDelegate::closeEditor, this, &FKProxyDelegate::closeEditor);
            disconnect(oldDelegate, &QAbstractItemDelegate::commitData, this, &FKProxyDelegate::commitData);
            disconnect(oldDelegate, &QAbstractItemDelegate::sizeHintChanged, this, &FKProxyDelegate::sizeHintChanged);
        }
    }

    if (delegate) {
        connect(delegate, &QAbstractItemDelegate::closeEditor, this, &FKProxyDelegate::closeEditor);
        connect(delegate, &QAbstractItemDelegate::commitData, this, &FKProxyDelegate::commitData);
        connect(delegate, &QAbstractItemDelegate::sizeHintChanged, this, &FKProxyDelegate::sizeHintChanged);

        m_delegates.insert(column, delegate);
    }
}

QAbstractItemDelegate *FKProxyDelegate::delegateForColumn(int column) const
{
    if (m_delegates.contains(column)) {
        return m_delegates[column];
    }
    return nullptr;
}
