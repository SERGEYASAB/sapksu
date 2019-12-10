#ifndef DEVICESTABWIDGET_H
#define DEVICESTABWIDGET_H

#include "Common/PrefixHeader.h"

class DevicesTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit DevicesTabWidget(QWidget *parent = nullptr);

    enum Equipment {
        Controller  = 0,
        FC          = 1,
        Voltmeters  = 2,
        NL16DO      = 3,
        MKTN        = 4,
        UPS         = 5
    };
    Q_ENUM(Equipment)

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // DEVICESTABWIDGET_H
