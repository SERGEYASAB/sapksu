#include "ParameterAliasWidgetDelegate.h"
#include "ParameterAliasWidget.h"

ParameterAliasWidgetDelegate::ParameterAliasWidgetDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget *ParameterAliasWidgetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return QStyledItemDelegate::createEditor(parent, option, index);

    ParameterAliasWidget *editor = new ParameterAliasWidget(parent);
    return editor;
}

void ParameterAliasWidgetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!index.isValid())
        return QStyledItemDelegate::setEditorData(editor, index);

    ParameterAliasWidget *e = qobject_cast<ParameterAliasWidget *>(editor);
    e->setupAlias(index.data().toString());
}

void ParameterAliasWidgetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    ParameterAliasWidget *e = qobject_cast<ParameterAliasWidget *>(editor);

    if (!index.isValid() || !e || !model)
        return QStyledItemDelegate::setModelData(editor, model, index);

    model->setData(index, e->selectedAlias());
}

bool ParameterAliasWidgetDelegate::eventFilter(QObject *watched, QEvent *event)
{
    ParameterAliasWidget *editor = qobject_cast<ParameterAliasWidget *>(watched);
    if (editor && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->matches(QKeySequence::Backspace) || keyEvent->matches(QKeySequence::Delete)) {
            editor->clearAlias();
            return true;
        }
    }
    return QStyledItemDelegate::eventFilter(watched, event);
}
