#include "HexConvertDelegate.h"

HexConvertDelegate::HexConvertDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void HexConvertDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    int value = index.model()->data(index).toInt();

    QString displayText;
    QTextStream stream(&displayText);
    stream.setNumberFlags(QTextStream::UppercaseDigits);

    stream << left << "0x";

    stream.setFieldWidth(8);
    stream.setPadChar('0');

    stream << right << hex << value;
    stream.flush();

    opt.text = displayText;

    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}
