#ifndef DEVICE_H
#define DEVICE_H

#include "Common/PrefixHeader.h"

class QTableView;
class DeviceModel;
class DeviceDelegate;
class INode;
class QLabel;

class IVariableDelegate;

class Device : public QWidget
{
    Q_OBJECT

public:
    explicit Device(QWidget *parent = Q_NULLPTR);
    Device(const INode *node = Q_NULLPTR, QWidget *parent = Q_NULLPTR);

    void setDataSourceNode(const INode *node);
    QTableView *getView();

private:
    DeviceModel *_model = nullptr;
    IVariableDelegate *_delegate = nullptr;

    QLabel *_nodeNameLabel = nullptr;
    QTableView *_view = nullptr;
};

#endif // DEVICE_H
