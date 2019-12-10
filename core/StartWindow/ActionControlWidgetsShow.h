#ifndef ACTIONCONTROLWIDGETSSHOW_H
#define ACTIONCONTROLWIDGETSSHOW_H

#include "Common/PrefixHeader.h"

class ActionControlWidgetsShow : public QObject
{
    Q_OBJECT

public:
    static ActionControlWidgetsShow *instance();
    static void showAdjustWidget(QWidget *parentWidget, QWidget *childWidget, QSize size = QSize(), bool maximized = false);

private:

    ActionControlWidgetsShow(ActionControlWidgetsShow const&) = delete;             // Copy construct
    ActionControlWidgetsShow(ActionControlWidgetsShow&&) = delete;                  // Move construct
    ActionControlWidgetsShow& operator=(ActionControlWidgetsShow const&) = delete;  // Copy assign
    ActionControlWidgetsShow& operator=(ActionControlWidgetsShow &&) = delete;      // Move assign

protected:
    explicit ActionControlWidgetsShow();
    virtual ~ActionControlWidgetsShow();
};

#endif // ACTIONCONTROLWIDGETSSHOW_H
