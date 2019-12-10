#ifndef RIZELECTON08WIDGET_H
#define RIZELECTON08WIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class RizElecton08Widget : public QWidget
{
    Q_OBJECT
public:
    explicit RizElecton08Widget(QWidget *parent = nullptr);
};

#endif // RIZELECTON08WIDGET_H
