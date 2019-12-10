#include "TestInternalVariableWidget.h"
#include "TestInternalVariableModel.h"
#include "TestInternalVariableDelegate.h"
#include "IOServer/nsVariable/IVariable.h"

TestInternalVariableWidget::TestInternalVariableWidget(QAbstractItemDelegate *delegate, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    setLayout(mainLay);

    m_model = new TestInternalVariableModel();

    m_view = new QTableView(this);
    m_view->setModel(m_model);
    mainLay->addWidget(m_view);

    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->setItemDelegate(new TestInternalVariableDelegate);
//    if (delegate)
    //   m_view->setItemDelegate(new QItemDelegate);

    QHeaderView *verticalHeader = m_view->verticalHeader();
    verticalHeader->setDefaultSectionSize(25);
    verticalHeader->hide();
}

void TestInternalVariableWidget::setData(QList<EngineLocalVariable *> vars)
{
    m_model->setDataSource(vars);
}
