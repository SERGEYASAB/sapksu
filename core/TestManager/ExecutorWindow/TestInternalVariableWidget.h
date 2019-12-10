#ifndef TESTINTERNALVARIABLEWIDGET_H
#define TESTINTERNALVARIABLEWIDGET_H

#include "Common/PrefixHeader.h"

class TestInternalVariableModel;
class EngineLocalVariable;

class TestInternalVariableWidget : public QWidget
{
    Q_OBJECT

public:
    TestInternalVariableWidget(QAbstractItemDelegate *delegate, QWidget *parent = Q_NULLPTR);
    void setData(QList<EngineLocalVariable *> vars);

private:
    QTableView *m_view;
    TestInternalVariableModel *m_model;
};

#endif // TESTINTERNALVARIABLEWIDGET_H
