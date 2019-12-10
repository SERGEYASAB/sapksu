#include "ModbusKsuManualTestingValueDelegate.h"

#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/EnumVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/BitsetVariableValueStrategy.h"

#include "Common/FKValidatorSpinBox/FKInt64Validator.h"
#include "Common/FKValidatorSpinBox/FKUInt64Validator.h"
#include "Common/FKValidatorSpinBox/FKCP866StringValidator.h"

ModbusKsuManualTestingValueDelegate::ModbusKsuManualTestingValueDelegate(IVariableByIndex *model, QObject *parent) :
    QStyledItemDelegate(parent),
    m_model(model)
{

}

void ModbusKsuManualTestingValueDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    IVariable *variable = m_model->variableByIndex(index);
//    if (variable &&
//            (variable->valueStrategyType() == IVariable::EnumStrategy ||
//             variable->valueStrategyType() == IVariable::BitsetStrategy)) {
//        drawItemText(painter, option, index, index.data().toString());
//        return;
//    }

    IVariable *variable = m_model->variableByIndex(index);
    if (variable && variable->serverValueDataType() == IVariable::DataType::DateTime) {
        QVariant value = index.data();
        QDateTime dateTime = value.toDateTime();
        if (dateTime.isValid()) {
            QString dateText = dateTime.toString(QStringLiteral("dd.MM.yyyy HH:mm:ss"));
            drawItemText(painter, option, index, dateText);
            return;
        }
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QWidget *ModbusKsuManualTestingValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    IVariable *variable = m_model->variableByIndex(index);
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
    } else if (variable->serverValueDataType() == IVariable::DataType::DateTime) {
        QDateTime minimumDateTime(QDate(1900, 1, 1), QTime(0, 0, 0));
        QDateTime maximumDateTime(QDate(2155, 12, 31), QTime(23, 59, 59));

        QDateTimeEdit *editor = new QDateTimeEdit(QDate::currentDate(), parent);
        editor->setMinimumDateTime(minimumDateTime);
        editor->setMaximumDateTime(maximumDateTime);
        editor->setDisplayFormat(QStringLiteral("dd.MM.yyyy HH:mm:ss"));
        editor->setCalendarPopup(true);
        return editor;
    } else if (variable->serverValueDataType() == IVariable::DataType::String) {
        QLineEdit *editor = new QLineEdit(parent);
        FKCP866StringValidator *validator = new FKCP866StringValidator(variable->precision() * 2, editor);
        editor->setValidator(validator);
        return editor;
    }

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

void ModbusKsuManualTestingValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    IVariable *variable = m_model->variableByIndex(index);
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
    } else if (QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>(editor)) {
        QDateTime valueDateTime = value.toDateTime();
        if (valueDateTime.isValid()) {
            dateTimeEdit->setDateTime(valueDateTime);
        }
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void ModbusKsuManualTestingValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    IVariable *variable = m_model->variableByIndex(index);
    if (!variable)
        return;

    if (QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>(editor)) {
        QDateTime dateTime = dateTimeEdit->dateTime();
        model->setData(index, dateTime);
    } else if (QComboBox *comboBox = qobject_cast<QComboBox *>(editor)) {
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
        if (ok) {
            model->setData(index, enumKey);
        }
    } else if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor)) {
        if (lineEdit->hasAcceptableInput()) {
            bool ok = false;
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

                case IVariable::String:
                    settedValue = QVariant(lineEdit->text());
                    ok = true;
                    break;

                default:
                    break;
            }

            if (ok) {
                model->setData(index, settedValue);
            }
        }
    }
}

//bool ModbusKsuManualTestingValueDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
//{

//}

void ModbusKsuManualTestingValueDelegate::drawItemText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QString &text) const
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
