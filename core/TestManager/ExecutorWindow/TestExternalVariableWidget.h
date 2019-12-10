#ifndef TESTEXTERNALVARIABLEWIDGET_H
#define TESTEXTERNALVARIABLEWIDGET_H

#include "Common/PrefixHeader.h"

class TestExternalVariableModel;
class IVariable;

class TestExternalVariableWidget : public QWidget
{
    Q_OBJECT

public:
    TestExternalVariableWidget(QAbstractItemDelegate *delegate, QWidget *parent = Q_NULLPTR);
    void setData(QList<IVariable *> vars);

private:
    QTableView *m_view;
    TestExternalVariableModel *m_model;
};

#endif // TESTEXTERNALVARIABLEWIDGET_H
