#include "Ksu735Manual0x20TestingVariableDelegate.h"

#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

#include "Ksu735Manual0x20TestingParametr.h"
#include "Ksu735Manual0x20TestingModel.h"

Ksu735Manual0x20TestingVariableDelegate::Ksu735Manual0x20TestingVariableDelegate(Ksu735Node *node, Ksu735Manual0x20TestingModel *model, QObject *parent) :
    QStyledItemDelegate(parent),
    m_node(node),
    m_model(model)
{

}

QWidget *Ksu735Manual0x20TestingVariableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    Ksu735Manual0x20TestingParametr *parametr = m_model->parametrByIndex(index);
    if (!parametr)
        return nullptr;

    int address = -1;
    if (parametr->variable)
        address = parametr->variable->address();

    QSpinBox *spinBoxEditor = new QSpinBox(parent);
    spinBoxEditor->setMinimum(-1);
    spinBoxEditor->setMaximum(255);
    spinBoxEditor->setValue(address);

    initialVariableAddress = address;
    initialEditorIndex = index;

    connect(spinBoxEditor, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [this, index](int value) {
        m_model->setData(index, value);
    });

    return spinBoxEditor;
}

void Ksu735Manual0x20TestingVariableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Ksu735Manual0x20TestingParametr *parametr = m_model->parametrByIndex(index);
    if (!parametr)
        return;

    QSpinBox *spinBoxEditor = qobject_cast<QSpinBox *>(editor);
    if (!spinBoxEditor)
        return;

    int address = -1;
    if (parametr->variable)
        address = parametr->variable->address();

    spinBoxEditor->setValue(address);
}

void Ksu735Manual0x20TestingVariableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *spinBoxEditor = qobject_cast<QSpinBox *>(editor);
    if (spinBoxEditor) {
        m_model->setData(index, spinBoxEditor->value());
        initialVariableAddress = -1;
        initialEditorIndex = QModelIndex();
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

bool Ksu735Manual0x20TestingVariableDelegate::eventFilter(QObject *watched, QEvent *event)
{
    QSpinBox *editor = qobject_cast<QSpinBox *>(watched);
    if (editor && initialEditorIndex.isValid() && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->matches(QKeySequence::Cancel)) {
            m_model->setData(initialEditorIndex, initialVariableAddress);
            initialVariableAddress = -1;
            initialEditorIndex = QModelIndex();
        }
    }

    return QStyledItemDelegate::eventFilter(watched, event);
}
