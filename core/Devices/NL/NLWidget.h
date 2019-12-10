#ifndef NLWIDGET_H
#define NLWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class NLWidget : public QWidget
{
    Q_OBJECT
public:
    NLWidget(QWidget *parent = 0);

private slots:
    void configurationChanged();

private:
    INode *findNodeByName(QList<INode *> &container, const QString &nodeName) const;

    Device *nlRiz = nullptr;
};

#endif // NLWIDGET_H
