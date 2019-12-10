#ifndef HEXCONVERTDELEGATE_H
#define HEXCONVERTDELEGATE_H

#include "Common/PrefixHeader.h"

class HexConvertDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit HexConvertDelegate(QObject *parent = Q_NULLPTR);

public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // HEXCONVERTDELEGATE_H
