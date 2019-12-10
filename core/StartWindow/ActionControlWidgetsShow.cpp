#include "ActionControlWidgetsShow.h"

void ActionControlWidgetsShow::showAdjustWidget(QWidget *parentWidget, QWidget *childWidget, QSize size, bool maximized)
{
    childWidget->resize(size);
    childWidget->move(QApplication::desktop()->screenGeometry().center() - childWidget->rect().center());
    if (maximized)
        childWidget->showMaximized();
    else
        childWidget->show();
    childWidget->activateWindow();

    connect(childWidget, &QWidget::destroyed, parentWidget, &QWidget::showNormal);
}

ActionControlWidgetsShow::ActionControlWidgetsShow()
{

}

ActionControlWidgetsShow::~ActionControlWidgetsShow()
{

}
