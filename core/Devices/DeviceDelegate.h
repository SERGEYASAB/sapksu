#ifndef DEVICEDELEGATE_H
#define DEVICEDELEGATE_H

#include "Common/PrefixHeader.h"

class DeviceDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DeviceDelegate(QObject *parent = Q_NULLPTR);

    // Создает виджет редактора значения
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

//    // Передает значение от модели в редактор
//    void setEditorData(QWidget *editor, const QModelIndex &index) const override {};

//    // Передает значение от редактора в модель
//    void setModelData(QWidget *editor,
//                      QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    bool showConfirmChangeValueMessage(const QString &valueName) const;
};

#endif // DEVICEDELEGATE_H
