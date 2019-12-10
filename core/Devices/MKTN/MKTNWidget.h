#ifndef MKTNWIDGET_H
#define MKTNWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class MKTNWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MKTNWidget(QWidget *parent = 0, const QString &nodeName = "");

signals:

public slots:

private slots:
    void configurationChanged();

private:
    INode *findNodeByName(QList<INode *> &container, const QString &nodeName) const;

    Device *v1;
    QString m_nodeName;
};

#endif // MKTNWIDGET_H
