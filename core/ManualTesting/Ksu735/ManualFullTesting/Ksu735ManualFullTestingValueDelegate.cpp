#include "Ksu735ManualFullTestingValueDelegate.h"
#include "Ksu735ManualFullTestingModel.h"
#include "Ksu735ManualFullTestingParametr.h"

#include "IOServer/nsVariable/Ksu735Variable.h"
#include "Common/FKValidatorSpinBox/FKCP866StringValidator.h"

Ksu735ManualFullTestingValueDelegate::Ksu735ManualFullTestingValueDelegate(Ksu735ManualFullTestingModel *model, QObject *parent) :
    QStyledItemDelegate(parent),
    m_model(model)
{

}

void Ksu735ManualFullTestingValueDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Ksu735ManualFullTestingParametr *parametr = m_model->parametrByIndex(index);
    if (parametr && parametr->variable() && parametr->variable()->format() == SapKsu::CCCC) {
        bool ok;
        int value = index.data().toInt(&ok);
        if (ok) {
            union {
                quint16 value;
                char bytes[2];
            } valueToBytes;
            valueToBytes.value = value;

            auto decoder = QTextCodec::codecForName("cp866")->makeDecoder();
            QByteArray cp866Bytes;
            cp866Bytes.append(valueToBytes.bytes[1]);
            cp866Bytes.append(valueToBytes.bytes[0]);

            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);
            opt.text = decoder->toUnicode(cp866Bytes);

            const QWidget *widget = option.widget;
            QStyle *style = widget ? widget->style() : QApplication::style();
            style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
            return;
        }
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QWidget *Ksu735ManualFullTestingValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    Ksu735ManualFullTestingParametr *parametr = m_model->parametrByIndex(index);
    if (!parametr || !parametr->variable())
        return nullptr;

    SapKsu::Ksu735ParametrFormat format = parametr->variable()->format();
    QVariant variantValue = parametr->writedValue;

    bool ok;
    if (format == SapKsu::Ksu735ParametrFormat::CCCC) {
        QLineEdit *editor = new QLineEdit(parent);
        editor->setValidator(new FKCP866StringValidator(2, editor));
        int value = variantValue.toInt(&ok);
        if (ok) {
            union {
                quint16 value;
                char bytes[2];
            } valueToBytes;
            valueToBytes.value = value;

            auto decoder = QTextCodec::codecForName("cp866")->makeDecoder();
            QByteArray cp866Bytes;
            cp866Bytes.append(valueToBytes.bytes[1]);
            cp866Bytes.append(valueToBytes.bytes[0]);
            editor->setText(decoder->toUnicode(cp866Bytes));
        }
        return editor;
    } else if (format == SapKsu::Ksu735ParametrFormat::XXXX ||
               format >= SapKsu::Ksu735ParametrFormat::XX1XX) {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setMinimum(0);
        if (format == SapKsu::Ksu735ParametrFormat::XXXX) {
            editor->setMaximum(9999);
        } else {
            editor->setMaximum(65535);
        }
        editor->setLocale(QLocale(QLocale::C));
        editor->setWrapping(true);

        int value = variantValue.toInt(&ok);
        if (!ok) {
            editor->setValue(value);
        }

        return editor;
    } else {
        int divisor = SapKsu::paramertValueDivisorForFormat(format);
        double maxValue = double(9999) / divisor;

        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setLocale(QLocale(QLocale::C));
        editor->setMinimum(0);
        editor->setMaximum(maxValue);
        editor->setWrapping(true);
        editor->setDecimals(log10(divisor));
        editor->setSingleStep(double(1) / divisor);

        double value = variantValue.toDouble(&ok);
        if (!ok) {
            editor->setValue(value);
        }

        return editor;
    }
    return nullptr;
}

void Ksu735ManualFullTestingValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Ksu735ManualFullTestingParametr *parametr = m_model->parametrByIndex(index);
    if (parametr && parametr->variable() && parametr->variable()->format() == SapKsu::CCCC) {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        if (lineEdit) {
            bool ok;
            int value = parametr->writedValue.toInt(&ok);
            if (ok) {
                union {
                    quint16 value;
                    char bytes[2];
                } valueToBytes;
                valueToBytes.value = value;

                auto decoder = QTextCodec::codecForName("cp866")->makeDecoder();
                QByteArray cp866Bytes;
                cp866Bytes.append(valueToBytes.bytes[1]);
                cp866Bytes.append(valueToBytes.bytes[0]);
                lineEdit->setText(decoder->toUnicode(cp866Bytes));
                return;
            }
        }
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void Ksu735ManualFullTestingValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Ksu735ManualFullTestingParametr *parametr = m_model->parametrByIndex(index);
    if (parametr && parametr->variable() && parametr->variable()->format() == SapKsu::CCCC ) {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        if (lineEdit) {
            if (!lineEdit->hasAcceptableInput())
                return;

            QString utf8Text = lineEdit->text();
            if (utf8Text.isEmpty()) {
                model->setData(index, QVariant());
                return;
            }

            if (utf8Text.size() == 1)
                utf8Text.append(" ");

            auto encoder = QTextCodec::codecForName("cp866")->makeEncoder();
            QByteArray cp866Bytes = encoder->fromUnicode(utf8Text);

            QByteArray reversed = cp866Bytes;
            std::reverse(reversed.begin(), reversed.end());

            union {
                quint16 value;
                char bytes[2];
            } valueToBytes;

            for (int i = 0; i < 2; i++) {
                valueToBytes.bytes[i] = reversed.at(i);
            }

            model->setData(index, valueToBytes.value);
            return;
        }
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}
