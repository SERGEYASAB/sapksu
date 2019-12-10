#ifndef ICPCONWIDGET_H
#define ICPCONWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class IcpConWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IcpConWidget(QWidget *parent = 0);

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
};

#endif // ICPCONWIDGET_H
