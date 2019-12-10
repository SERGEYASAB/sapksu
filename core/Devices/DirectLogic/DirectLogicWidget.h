#ifndef DIRECTLOGICWIDGET_H
#define DIRECTLOGICWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class DirectLogicWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DirectLogicWidget(QWidget *parent = 0);

signals:

public slots:

private slots:
    void configurationChanged();

private:
    INode *findNodeByName(QList<INode *> &container, const QString &nodeName) const;

    Device *a15;
    Device *a16;
    Device *a17;
    Device *a18;
    Device *a19;
    Device *a22;
    Device *a23;
    Device *a24;
    Device *a25;
    Device *a26;
    Device *a27;
    Device *a28;
    Device *a29;
};

#endif // DIRECTLOGICWIDGET_H
