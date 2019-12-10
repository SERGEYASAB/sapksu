#ifndef UPSWIDGET_H
#define UPSWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class UPSWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UPSWidget(QWidget *parent = 0);

};

#endif // UPSWIDGET_H
