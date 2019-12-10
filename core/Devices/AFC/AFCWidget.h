#ifndef AFCWIDGET_H
#define AFCWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class AFCWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AFCWidget(QWidget *parent = 0);

    void setViewForManualTestingWidget();

signals:

public slots:

private slots:
    void configurationChanged();

private:
    INode *findNodeByName(QList<INode *> &container, const QString &nodeName) const;

    Device *v1;
    Device *v2;
    Device *v3;
};

#endif // AFCWIDGET_H
