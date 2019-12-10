#ifndef KSU735MANUAL0X20TESTINGVARIABLEDELEGATE_H
#define KSU735MANUAL0X20TESTINGVARIABLEDELEGATE_H

#include "Common/PrefixHeader.h"

class Ksu735Node;
class Ksu735Manual0x20TestingModel;

class Ksu735Manual0x20TestingVariableDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    Ksu735Manual0x20TestingVariableDelegate(Ksu735Node *node,
                                            Ksu735Manual0x20TestingModel *model,
                                            QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ksu735Node *m_node = nullptr;
    Ksu735Manual0x20TestingModel *m_model;

    mutable int initialVariableAddress = -1;
    mutable QModelIndex initialEditorIndex = QModelIndex();
};

#endif // KSU735MANUAL0X20TESTINGVARIABLEDELEGATE_H
