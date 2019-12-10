#include "ModbusKsuManualSelectiveTestingVariableDelegate.h"

#include "IOServer/nsNode/ModbusKsuNode.h"
#include "IOServer/nsVariable/ModbusVariable.h"

#include "ModbusKsuManualSelectiveTestingModel.h"
#include "ModbusKsuManualSelectiveTestingParametr.h"

#include "ModbusVariableAddressSelectSpinBox.h"


ModbusKsuManualSelectiveTestingVariableDelegate::ModbusKsuManualSelectiveTestingVariableDelegate(
        ModbusKsuNode *node,
        ModbusKsuManualSelectiveTestingModel *model,
        QObject *parent) :

    QStyledItemDelegate(parent),
    m_node(node),
    m_model(model)
{

}

QWidget *ModbusKsuManualSelectiveTestingVariableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    ModbusKsuManualSelectiveTestingParametr *parametr = m_model->parametrByIndex(index);
    if (!parametr)
        return nullptr;

    int address = -1;
    if (parametr->variable)
        address = parametr->variable->address();

    ModbusVariableAddressSelectSpinBox *spinBoxEditor = new ModbusVariableAddressSelectSpinBox(m_node, parent);
    spinBoxEditor->setMinimum(-1);
    spinBoxEditor->setMaximum(65535);
    spinBoxEditor->setValue(address);
    spinBoxEditor->setWrapping(true);

    initialVariableAddress = address;
    initialEditorIndex = index;

    connect(spinBoxEditor, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [this, index](int value) {
        m_model->setData(index, value);
    });

    return spinBoxEditor;
}

void ModbusKsuManualSelectiveTestingVariableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    ModbusKsuManualSelectiveTestingParametr *parametr = m_model->parametrByIndex(index);
    if (!parametr)
        return;

    ModbusVariableAddressSelectSpinBox *spinBoxEditor = qobject_cast<ModbusVariableAddressSelectSpinBox *>(editor);
    if (!spinBoxEditor)
        return;

    int address = -1;
    if (parametr->variable)
        address = parametr->variable->address();
    else
        address = parametr->address;

    spinBoxEditor->setValue(address);
}

void ModbusKsuManualSelectiveTestingVariableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    ModbusVariableAddressSelectSpinBox *spinBoxEditor = qobject_cast<ModbusVariableAddressSelectSpinBox *>(editor);
    if (spinBoxEditor) {
        m_model->setData(index, spinBoxEditor->value());
        initialVariableAddress = -1;
        initialEditorIndex = QModelIndex();
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

bool ModbusKsuManualSelectiveTestingVariableDelegate::eventFilter(QObject *watched, QEvent *event)
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
