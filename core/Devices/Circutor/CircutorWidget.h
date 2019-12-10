#ifndef CIRCUTORWIDGET_H
#define CIRCUTORWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class CircutorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CircutorWidget(bool attestation = false, QWidget *parent = 0);
    void setInvisibleDevice(int deviceNumber, bool value);
signals:

public slots:

private slots:
    void configurationChanged();

private:
    INode *findNodeByName(QList<INode *> &container, const QString &nodeName) const;

    Device *v1;
    Device *v2;
    Device *v3;

    bool _attestation = false;
};

#endif // CIRCUTORWIDGET_H
