#include "DeviceDelegate.h"

#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsVariable/VariableValue/ServerVariableValue.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/NumberVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/EnumVariableValueStrategy.h"

#include <QComboBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QValidator>
#include <QStyleOptionViewItemV4>

DeviceDelegate::DeviceDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{
}

QWidget *DeviceDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *editor = nullptr;
    IVariable *variable = static_cast<IVariable *>(index.internalPointer());
    if (EnumVariableValueStrategy *strategy = dynamic_cast<EnumVariableValueStrategy *>(variable->valueStrategy())) {
        QComboBox *comboBox = new QComboBox();
        auto enumValues = strategy->enumValues();
        auto i = enumValues.constBegin();
        while (i != enumValues.constEnd()) {
            comboBox->addItem(i.value(), i.key());
            ++i;
        }
        editor = comboBox;
    }/* else {
        IVariable::DataType = variable->serverValueDataType()
        if ()
    }*/


    if (editor) {
        editor->setParent(parent);
    } else {
        editor = QStyledItemDelegate::createEditor(parent, option, index);
    }

    return editor;
}

void DeviceDelegate::updateEditorGeometry(QWidget *editor,
                                          const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)

    editor->setGeometry(option.rect);
}

bool DeviceDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    IVariable *variable = static_cast<IVariable *>(index.internalPointer());
    if (variable && variable->isWritable() && variable->isBoolean() && index.flags() & Qt::ItemIsEditable) {
        if (event->type() == QEvent::MouseButtonDblClick || event->type() == QEvent::KeyPress) {
            if (event->type() == QEvent::KeyPress && dynamic_cast<QKeyEvent *>(event)->key() != Qt::Key_Space) {
                return true;
            }

            if (showConfirmChangeValueMessage(variable->path())) {
                model->setData(index, QVariant(!variable->value().toBool()));
            }
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);

    //    if ((event->type() == QEvent::MouseButtonDblClick || event->type() == QEvent::KeyPress)
    //            && index.column() == 1 && ()) {

    //            if (showConfirmChangeValueMessage(variable->path())) {
    //                model->setData(index, QVariant(!variable->value().toBool()));
    //            }
    //            return true;
    //        }
    //    }
    //    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

bool DeviceDelegate::showConfirmChangeValueMessage(const QString &valueName) const
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(tr("Значение переменной \n%1\nбыло изменено").arg(valueName));
    msgBox.setInformativeText(tr("Сохранить изменения?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    return msgBox.exec() == QMessageBox::Ok;
}
