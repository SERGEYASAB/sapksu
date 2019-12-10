#include "Device.h"

#include "IOServer/nsNode/INode.h"
#include "Devices/DeviceModel.h"
#include "Devices/DeviceDelegate.h"
#include "IOServerUI/IVariableDelegate/IVariableDelegate.h"

Device::Device(QWidget *parent) : Device(nullptr, parent)
{

}

Device::Device(const INode *node, QWidget *parent) : QWidget(parent)
{
    _model = new DeviceModel(this);
    _delegate = new IVariableDelegate(this);

    _view = new QTableView(this);
    _view->setModel(_model);
    _view->setItemDelegateForColumn(1, _delegate);

    _nodeNameLabel = new QLabel(this);
    _nodeNameLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_nodeNameLabel);
    layout->addWidget(_view);
    layout->setMargin(5);
    layout->setSpacing(5);

    setLayout(layout);

    setDataSourceNode(node);

    _view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//    _view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _view->horizontalHeader()->setStretchLastSection(true);
//    _view->verticalHeader()->setDefaultSectionSize(25);

    QHeaderView *verticalHeader = _view->verticalHeader();
    verticalHeader->setDefaultSectionSize(25);
    verticalHeader->hide();
}

void Device::setDataSourceNode(const INode *node)
{
    _model->setDataSource(node);

    QString nodeName;
    QString nodeDescription;
    if (node) {
        nodeName = node->name();
        nodeDescription = node->description();
    }
    _nodeNameLabel->setText(nodeName.append(" — ").append(nodeDescription));
    _view->resizeColumnsToContents();
    _view->resizeRowsToContents();
}

QTableView *Device::getView()
{
    return _view;
}


