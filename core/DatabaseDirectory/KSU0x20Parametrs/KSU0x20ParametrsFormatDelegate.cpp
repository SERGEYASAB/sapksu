#include "KSU0x20ParametrsFormatDelegate.h"
#include "KSU0x20Parametr.h"

KSU0x20ParametrsFormatDelegate::KSU0x20ParametrsFormatDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void KSU0x20ParametrsFormatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    KSU0x20Parametr *parametr = static_cast<KSU0x20Parametr *>(index.internalPointer());
    QString formatText = index.data(Qt::DisplayRole).toString();
    if (parametr) {
        SapKsu::Ksu735ParametrFormat format = parametr->format();
        QMetaEnum formatEnum = QMetaEnum::fromType<SapKsu::Ksu735ParametrFormat>();
        formatText = formatEnum.key(format);
        formatText.replace('1', '/');
        formatText.replace('_', '.');
    }

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.text = formatText;

    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

QWidget *KSU0x20ParametrsFormatDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    KSU0x20Parametr *parametr = static_cast<KSU0x20Parametr *>(index.internalPointer());
    QComboBox *formatComboBox = new QComboBox(parent);

    QMetaEnum formatsMetaEnum = QMetaEnum::fromType<SapKsu::Ksu735ParametrFormat>();
    for (int i = 0; i < formatsMetaEnum.keyCount(); i++) {
        QString key = formatsMetaEnum.key(i);
        key.replace('_', '.');
        key.replace('1', '/');
        formatComboBox->addItem(key);
        if (parametr && parametr->format() == SapKsu::Ksu735ParametrFormat(formatsMetaEnum.value(i)))
            formatComboBox->setCurrentIndex(i);
    }

    return formatComboBox;
}

void KSU0x20ParametrsFormatDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void KSU0x20ParametrsFormatDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *formatComboBox = dynamic_cast<QComboBox *>(editor);
    if (!formatComboBox) return;

    model->setData(index, formatComboBox->currentIndex());
    return;
}
