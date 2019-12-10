#include "KSU0x20ParametrsDelegate.h"
#include "KSU0x20Parametr.h"

KSU0x20ParametrsDelegate::KSU0x20ParametrsDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void KSU0x20ParametrsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 4) {
        KSU0x20Parametr *parametr = static_cast<KSU0x20Parametr *>(index.internalPointer());
        if (parametr) {
            SapKsu::Ksu735ParametrFormat format = parametr->format();
            QMetaEnum formatEnum = QMetaEnum::fromType<SapKsu::Ksu735ParametrFormat>();
            QString formatString = formatEnum.key(format);
            formatString.replace('1', '/1');
            formatString.replace('_', '.1');

            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);
            opt.text = formatString;

            const QWidget *widget = option.widget;
            QStyle *style = widget ? widget->style() : QApplication::style();
            style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

//            QStyleOptionViewItem itemOption(option);
//            initStyleOption(&itemOption, index);
//            itemOption.text = formatString;  // override text


////            if (option.state & QStyle::State_Selected) {
////                const QBrush &br = option.palette.highlight();
////                qDebug() << br.style() << bool(option.palette == itemOption.palette);
////                painter->fillRect(option.rect, br);
////                itemOption.features &= ~(1 << QStyleOptionViewItem::HasDecoration);
////                itemOption.features &= ~(1 << QStyleOptionViewItem::HasDisplay);
////                itemOption.features |= QStyleOptionViewItem::ViewItemFeature(1 << QStyleOptionViewItem::HasDecoration);
////                itemOption.features |= QStyleOptionViewItem::ViewItemFeature(1 << QStyleOptionViewItem::HasDisplay);
////                //                itemOption.showDecorationSelected = false;
////            }

//            const QWidget *widget = itemOption.widget;
//            qDebug() << "style" << itemOption.widget;
//            QStyle *style = widget ? widget->style() : QApplication::style();

//            style->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);

//            //            painter->drawText(option.rect, option.displayAlignment, formatString);

//            return;

            ////            option.text = "";
            //            QStyle *style = option.widget ? option.widget->style() : QApplication::style();
            //            style->drawControl(QStyle::CE_ItemViewItem, &option, painter, option.widget);

            //            QPalette::ColorGroup colorGroup = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
            //            if (option.state & QStyle::State_Selected)
            //                painter->setPen(option.palette.color(colorGroup, QPalette::HighlightedText));
            //            else
            //                painter->setPen(option.palette.color(colorGroup, QPalette::Text));

            //            painter->drawText(option.rect, option.displayAlignment, formatString);
            //            qDebug() << "option" << option;
            //            return;
        } else {
            if (option.state & QStyle::State_Selected)
                painter->fillRect(option.rect, option.palette.highlight());
        }



        //        QString line0 = index.model()->data(index.model()->index(index.row(), 1)).toString();
        //        QString line1 = index.model()->data(index.model()->index(index.row(), 2)).toString();

        //        // draw correct background
        //        opt.text = "";
        //        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        //        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        //        QRect rect = opt.rect;
        //        QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
        //        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
        //            cg = QPalette::Inactive;

        //        // set pen color
        //        if (opt.state & QStyle::State_Selected)
        //            painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
        //        else
        //            painter->setPen(opt.palette.color(cg, QPalette::Text));

        //        // draw 2 lines of text
        //        painter->drawText(QRect(rect.left(), rect.top(), rect.width(), rect.height()/2),
        //                          opt.displayAlignment, line0);
        //        painter->drawText(QRect(rect.left(), rect.top()+rect.height()/2, rect.width(), rect.height()/2),
        //                          opt.displayAlignment, line1);
    } else
    return QStyledItemDelegate::paint(painter, option, index);
}

QSize KSU0x20ParametrsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0) {
        return QSize(10, 0);
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *KSU0x20ParametrsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    KSU0x20Parametr *parametr = static_cast<KSU0x20Parametr *>(index.internalPointer());

    if (index.column() == 4) {
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
    if (index.column() == 5) {
        QComboBox *accessComboBox = new QComboBox(parent);
        accessComboBox->addItems(QStringList() << "R" << "R/W");
        if (parametr && parametr->writable())
            accessComboBox->setCurrentIndex(1);
        return accessComboBox;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void KSU0x20ParametrsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void KSU0x20ParametrsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == 4) {
        QComboBox *formatComboBox = dynamic_cast<QComboBox *>(editor);
        if (!formatComboBox) return;

        model->setData(index, formatComboBox->currentIndex());
        return;
    } if (index.column() == 5) {
        QComboBox *accessComboBox = dynamic_cast<QComboBox *>(editor);
        if (!accessComboBox) return;
        bool writable = accessComboBox->currentIndex() == 1;

        model->setData(index, writable);
        return;
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

bool KSU0x20ParametrsDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::KeyPress && dynamic_cast<QKeyEvent *>(event)->key() == Qt::Key_Space) {
        KSU0x20Parametr *parametr = static_cast<KSU0x20Parametr *>(index.internalPointer());
        if (parametr) {
            model->setData(model->index(index.row(), 0), !parametr->enabled(), Qt::CheckStateRole);
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
