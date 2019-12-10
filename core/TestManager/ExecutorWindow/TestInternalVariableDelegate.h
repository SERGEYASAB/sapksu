#ifndef TESTINTERNALVARIABLEDELEGATE_H
#define TESTINTERNALVARIABLEDELEGATE_H

#include "Common/PrefixHeader.h"

class TestInternalVariableDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TestInternalVariableDelegate(QObject *parent = Q_NULLPTR);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    bool showConfirmChangeValueMessage(const QString &valueName) const;
};

#endif // TESTINTERNALVARIABLEDELEGATE_H
