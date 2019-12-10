#include "IVariableDelegate.h"
//257,69
#include "IOServer/nsVariable/IVariable.h"

#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/EnumVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/BitsetVariableValueStrategy.h"

#include "Common/FKValidatorSpinBox/FKInt64Validator.h"
#include "Common/FKValidatorSpinBox/FKUInt64Validator.h"
#include "Common/FKValidatorSpinBox/FKCP866StringValidator.h"

IVariableDelegate::IVariableDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void IVariableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    IVariable *variable = static_cast<IVariable *>(index.internalPointer());
    if (variable &&
            (variable->valueStrategyType() == IVariable::EnumStrategy ||
             variable->valueStrategyType() == IVariable::BitsetStrategy)) {
        drawItemText(painter, option, index, variable->descriptionValue().toString());
        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QWidget *IVariableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    IVariable *variable = static_cast<IVariable *>(index.internalPointer());
    if (!variable)
        return nullptr;

    EnumVariableValueStrategy *enumStrategy = dynamic_cast<EnumVariableValueStrategy *>(variable->valueStrategy());
    if (variable->valueStrategyType() == IVariable::EnumStrategy && enumStrategy) {
        QComboBox *editor = new QComboBox(parent);

        bool ok;
        quint64 value = variable->value().toULongLong(&ok);

        auto enumValues = enumStrategy->enumValues();
        int index = -1;
        int counter = 0;
        for (auto it = enumValues.cbegin(); it != enumValues.cend(); it++) {
            editor->addItem(it.value(), it.key());
            if (ok && it.key() == value) {
                index = counter;
            }
            counter++;
        }
        editor->setCurrentIndex(index);
        return editor;
    }

    if (variable->serverValueDataType() == IVariable::Boolean) {
        return nullptr;
    }/* else if (variable->serverValueDataType() == IVariable::DataType::DateTime) {
        QDateTime minimumDateTime(QDate(1900, 1, 1), QTime(0, 0, 0));
        QDateTime maximumDateTime(QDate(2155, 12, 31), QTime(23, 59, 59));

        QDateTimeEdit *editor = new QDateTimeEdit(QDate::currentDate(), parent);
        editor->setMinimumDateTime(minimumDateTime);
        editor->setMaximumDateTime(maximumDateTime);
        editor->setDisplayFormat(QStringLiteral("dd.MMM.yyyy HH:mm:ss"));
        return editor;
    } else if (variable->serverValueDataType() == IVariable::DataType::String) {
        QLineEdit *editor = new QLineEdit(parent);
        FKCP866StringValidator *validator = new FKCP866StringValidator(variable->precision() * 2, editor);
        editor->setValidator(validator);
        return editor;
    }*/

    bool useMinMax = variable->valueStrategy()->useMinMax();
    QVariant minValue = variable->minValue();
    QVariant maxValue = variable->maxValue();
    uint precision = variable->precision();
    QLineEdit *editor = new QLineEdit(parent);

    switch (variable->serverValueDataType()) {
        case IVariable::Int8:
            Sap::setEditorValidator<qint8>(editor, useMinMax, minValue, maxValue);
            break;

        case IVariable::Int16:
            Sap::setEditorValidator<qint16>(editor, useMinMax, minValue, maxValue);
            break;

        case IVariable::Int32:
            Sap::setEditorValidator<qint32>(editor, useMinMax, minValue, maxValue);
            break;

        case IVariable::Int64:
            Sap::setEditorValidator<qint64>(editor, useMinMax, minValue, maxValue);
            break;

        case IVariable::UInt8:
            Sap::setEditorValidator<quint8>(editor, useMinMax, minValue, maxValue);
            break;

        case IVariable::UInt16:
            Sap::setEditorValidator<quint16>(editor, useMinMax, minValue, maxValue);
            break;

        case IVariable::UInt32:
            Sap::setEditorValidator<quint32>(editor, useMinMax, minValue, maxValue);
            break;

        case IVariable::UInt64:
            Sap::setEditorValidator<quint64>(editor, useMinMax, minValue, maxValue);
            break;

        case IVariable::Float32:
            Sap::setFloatEditorValidator(editor, useMinMax, minValue, maxValue, precision);
            break;

        case IVariable::Float64:
            Sap::setDoubleEditorValidator(editor, useMinMax, minValue, maxValue, precision);
            break;

        default:
            break;
    }

    return editor;
}

void IVariableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    IVariable *variable = static_cast<IVariable *>(index.internalPointer());
    if (!variable) {
        QStyledItemDelegate::setEditorData(editor, index);
        return;
    }

    QVariant value = index.data(Qt::EditRole);

    if (QComboBox *comboBox = qobject_cast<QComboBox *>(editor)) {
        int comboBoxIndex = comboBox->findData(value);
        if (comboBoxIndex == -1) {
            EnumVariableValueStrategy *enumStrategy = dynamic_cast<EnumVariableValueStrategy *>(variable->valueStrategy());
            if (variable->valueStrategyType() == IVariable::EnumStrategy && enumStrategy) {
                bool ok;
                quint64 enumKey = value.toULongLong(&ok);
                if (ok && enumStrategy->enumValues().contains(enumKey)) {
                    QString enumValue = enumStrategy->enumValues().value(enumKey);
                    comboBoxIndex = comboBox->findData(enumValue,
                                                       Qt::DisplayRole,
                                                       static_cast<Qt::MatchFlags>(Qt::MatchFixedString |
                                                                                   Qt::MatchCaseSensitive));
                }
            }
        }

        comboBox->setCurrentIndex(comboBoxIndex);
        return;
    } else if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor)) {
        lineEdit->setText(value.toString());
        return;
    } /*else if (QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>(editor)) {
        QDateTime valueDateTime = value.toDateTime();
        if (valueDateTime.isValid()) {
            dateTimeEdit->setDateTime(valueDateTime);
        }
    }*/

    QStyledItemDelegate::setEditorData(editor, index);
}

void IVariableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    IVariable *variable = static_cast<IVariable *>(index.internalPointer());
    if (!variable || !variable->isWritable())
        return;

    if (QComboBox *comboBox = qobject_cast<QComboBox *>(editor)) {
        if (comboBox->currentIndex() == -1) return;

        QVariant settedValue = comboBox->currentData();


        if (!settedValue.isValid() || settedValue.isNull() || !settedValue.canConvert<quint64>()) {
            EnumVariableValueStrategy *enumStrategy = dynamic_cast<EnumVariableValueStrategy *>(variable->valueStrategy());
            if (variable->valueStrategyType() == IVariable::EnumStrategy && enumStrategy) {
                auto keys = enumStrategy->enumValues().keys(comboBox->currentData(Qt::DisplayRole).toString());
                if (!keys.isEmpty()) {
                    settedValue.setValue(keys.first());
                }
            }
        }

        bool ok;
        quint64 enumKey = settedValue.toULongLong(&ok);
        if (ok && variable->value() != QVariant(enumKey)) {
            if (showConfirmChangeValueMessage(variable->name())) {
                variable->setValue(enumKey);
            }
        }
    } else if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor)) {
        if (lineEdit->hasAcceptableInput()) {
            bool ok;
            QVariant settedValue = QVariant();

            switch (variable->serverValueDataType()) {
                case IVariable::Boolean:
                    return;
                    break;

                case IVariable::Int8:
                    settedValue = QVariant(lineEdit->text().toShort(&ok, 0));
                    break;

                case IVariable::Int16:
                    settedValue = QVariant(lineEdit->text().toShort(&ok, 0));
                    break;

                case IVariable::Int32:
                    settedValue = QVariant(lineEdit->text().toInt(&ok, 0));
                    break;

                case IVariable::Int64:
                    settedValue = QVariant(lineEdit->text().toLongLong(&ok, 0));
                    break;

                case IVariable::UInt8:
                    settedValue = QVariant(lineEdit->text().toUShort(&ok, 0));
                    break;

                case IVariable::UInt16:
                    settedValue = QVariant(lineEdit->text().toUShort(&ok, 0));
                    break;

                case IVariable::UInt32:
                    settedValue = QVariant(lineEdit->text().toUInt(&ok, 0));
                    break;

                case IVariable::UInt64:
                    settedValue = QVariant(lineEdit->text().toULongLong(&ok, 0));
                    break;

                case IVariable::Float32:
                    settedValue = QVariant(lineEdit->text().toFloat(&ok));
                    break;

                case IVariable::Float64:
                    settedValue = QVariant(lineEdit->text().toDouble(&ok));
                    break;
            }

            if (ok && variable->value() != settedValue) {
                if (showConfirmChangeValueMessage(variable->name() + " (\"" + variable->description() + "\")")) {
                    variable->setValue(settedValue);
                }
            }
        }
    }
}

bool IVariableDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    IVariable *variable = static_cast<IVariable *>(index.internalPointer());
    if (variable && variable->isWritable() && variable->isBoolean() && index.flags() & Qt::ItemIsEditable) {
        if (event->type() == QEvent::MouseButtonDblClick || event->type() == QEvent::KeyPress) {
            if (event->type() == QEvent::KeyPress && dynamic_cast<QKeyEvent *>(event)->key() != Qt::Key_Space) {
                return true;
            }

            QVariant settedValue = variable->value();
            if (showConfirmChangeValueMessage(variable->name() + " (\"" + variable->description() + "\")")) {
                model->setData(index, QVariant(!settedValue.toBool()));
            }
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void IVariableDelegate::drawItemText(QPainter *painter, const QStyleOptionViewItem &option,
                                     const QModelIndex &index, const QString &text) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.text = text;

    painter->save();

    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    painter->restore();
}

bool IVariableDelegate::showConfirmChangeValueMessage(const QString &valueName) const
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(tr("Значение переменной \n%1\nбыло изменено").arg(valueName));
    msgBox.setInformativeText(tr("Сохранить изменения?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    return msgBox.exec() == QMessageBox::Ok;
}
