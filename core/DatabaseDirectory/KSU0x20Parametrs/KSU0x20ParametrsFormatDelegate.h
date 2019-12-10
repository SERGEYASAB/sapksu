 #ifndef KSU0X20PARAMETRSFORMATDELEGATE_H
#define KSU0X20PARAMETRSFORMATDELEGATE_H

#include "Common/PrefixHeader.h"

class KSU0x20ParametrsFormatDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    KSU0x20ParametrsFormatDelegate(QObject *parent = nullptr);

public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};

#endif // KSU0X20PARAMETRSFORMATDELEGATE_H
