#ifndef ACTIONSTYLEDGROUPMENUWIDGET_H
#define ACTIONSTYLEDGROUPMENUWIDGET_H

#include "Common/PrefixHeader.h"
class ActionStyledGroupMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActionStyledGroupMenuWidget(const QString &name, QWidget *parent = Q_NULLPTR);
    explicit ActionStyledGroupMenuWidget(const QString &name, int fontSize, QWidget *parent = Q_NULLPTR);
};

#endif // ACTIONSTYLEDGROUPMENUWIDGET_H
