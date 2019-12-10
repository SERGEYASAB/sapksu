#ifndef VOLTMETERWIDGET_H
#define VOLTMETERWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class VoltmeterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VoltmeterWidget(QWidget *parent = 0);

signals:

public slots:

private slots:
    void configurationChanged();

private:
    INode *findNodeByName(QList<INode *> &container, const QString &nodeName) const;

    Device *v1;
    Device *v2;
    Device *v3;
    Device *v4;
    Device *v5;
};

#endif // VOLTMETERWIDGET_H
