#ifndef ICPCONMECHWIDGET_H
#define ICPCONMECHWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class IcpConMechWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IcpConMechWidget(QWidget *parent = 0);

signals:

public slots:

private slots:
    void configurationChanged();

private:
    INode *findNodeByName(QList<INode *> &container, const QString &nodeName) const;

    Device *slot0;
    Device *slot1;
    Device *slot2;
    Device *slot3;

    Device *slot4;
    Device *slot5;
    Device *slot6;
};

#endif // ICPCONMECHWIDGET_H
