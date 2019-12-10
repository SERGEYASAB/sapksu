#include "TestExternalVariableWidget.h"
#include "TestExternalVariableModel.h"
#include "Devices/DeviceDelegate.h"
#include "IOServer/nsVariable/IVariable.h"

TestExternalVariableWidget::TestExternalVariableWidget(QAbstractItemDelegate *delegate, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    setLayout(mainLay);

    m_model = new TestExternalVariableModel();

    m_view = new QTableView(this);
    m_view->setModel(m_model);
    mainLay->addWidget(m_view);

    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setStretchLastSection(true);

    if (delegate)
        m_view->setItemDelegate(delegate);

    QHeaderView *verticalHeader = m_view->verticalHeader();
    verticalHeader->setDefaultSectionSize(25);
    verticalHeader->hide();
}

void TestExternalVariableWidget::setData(QList<IVariable *> vars)
{
    m_model->setDataSource(vars);
}
