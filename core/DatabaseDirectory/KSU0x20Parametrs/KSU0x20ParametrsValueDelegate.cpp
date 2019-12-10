#include "KSU0x20ParametrsValueDelegate.h"

KSU0x20ParametrsValueDelegate::KSU0x20ParametrsValueDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void KSU0x20ParametrsValueDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    KSU0x20Parametr *parametr = static_cast<KSU0x20Parametr *>(index.internalPointer());
    QString displayText = index.data(Qt::DisplayRole).toString();

    bool ok = false;
    quint16 value = displayText.toUInt(&ok);
    if (parametr && ok) {
        SapKsu::Ksu735ParametrFormat format = parametr->format();
        int divisor = SapKsu::paramertValueDivisorForFormat(format);

//        if (format == SapKsu::Ksu735ParametrFormat::CCCC) {
//            union {
//                quint16 value;
//                char bytes[2];
//            } valueToBytes;
//            valueToBytes.value = value;

//            auto decoder = QTextCodec::codecForName("cp866")->makeDecoder();
//            QByteArray cp866Bytes;
//            cp866Bytes.append(valueToBytes.bytes[1]);
//            cp866Bytes.append(valueToBytes.bytes[0]);
//            displayText = decoder->toUnicode(cp866Bytes);
//        }
        if (divisor != 1) {
            double fixedValue = double(value) / divisor;
            displayText = QString::number(fixedValue, 'f', log10(divisor));
        }
    }

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.text = displayText;

    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

QWidget *KSU0x20ParametrsValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    KSU0x20Parametr *parametr = static_cast<KSU0x20Parametr *>(index.internalPointer());
    QString displayText = index.data(Qt::DisplayRole).toString();

    bool ok = false;
    quint16 value = displayText.toUInt(&ok);
    if (parametr && ok) {
        SapKsu::Ksu735ParametrFormat format = parametr->format();
        if (format == SapKsu::Ksu735ParametrFormat::CCCC) {
            return nullptr;
        } else if (format == SapKsu::Ksu735ParametrFormat::XXXX ||
                   format >= SapKsu::Ksu735ParametrFormat::XX1XX) {
            QSpinBox *editor = new QSpinBox(parent);
            editor->setMinimum(0);
            if (format == SapKsu::Ksu735ParametrFormat::XXXX) {
                editor->setMaximum(9999);
            } else {
                editor->setMaximum(65535);
            }
            editor->setWrapping(true);
            editor->setValue(value);
            return editor;
        } else {
            int divisor = SapKsu::paramertValueDivisorForFormat(format);
            double currentValue = double(value) / divisor;
            double maxValue = double(9999) / divisor;

            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setLocale(QLocale("C"));
            editor->setMinimum(0);
            editor->setMaximum(maxValue);
            editor->setWrapping(true);
            editor->setDecimals(log10(divisor));
            editor->setSingleStep(double(1) / divisor);
            editor->setValue(currentValue);
            return editor;
        }
    }
    return nullptr;
}

void KSU0x20ParametrsValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    KSU0x20Parametr *parametr = static_cast<KSU0x20Parametr *>(index.internalPointer());
    if (parametr) {
        SapKsu::Ksu735ParametrFormat format = parametr->format();

        if (format == SapKsu::Ksu735ParametrFormat::XXXX || format >= SapKsu::Ksu735ParametrFormat::XX1XX) {
            QSpinBox *spinBox = dynamic_cast<QSpinBox *>(editor);
            if (spinBox) {
                model->setData(index, spinBox->value());
            }
        } else {
            QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox *>(editor);
            if (spinBox) {
                double doubleValue = spinBox->value() * SapKsu::paramertValueDivisorForFormat(format);
                quint16 value = round(doubleValue);
                model->setData(index, value);
            }
        }
    }
}
