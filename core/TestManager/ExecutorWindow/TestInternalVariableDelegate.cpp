#include "TestInternalVariableDelegate.h"

TestInternalVariableDelegate::TestInternalVariableDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{
}

QWidget *TestInternalVariableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setRange(-5000000, 5000000);
    return editor;
}

bool TestInternalVariableDelegate::showConfirmChangeValueMessage(const QString &valueName) const
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(tr("Значение переменной \n%1\nбыло изменено").arg(valueName));
    msgBox.setInformativeText(tr("Сохранить изменения?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    return msgBox.exec() == QMessageBox::Ok;
}
