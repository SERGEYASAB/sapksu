#include "VoltmeterWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "Devices/Device.h"

VoltmeterWidget::VoltmeterWidget(QWidget *parent) : QWidget(parent)
{
    v1 = new Device(this);
    v2 = new Device(this);
    v3 = new Device(this);
    v4 = new Device(this);
    v5 = new Device(this);

    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(2);
    layout->setMargin(5);

    layout->addWidget(v1, 0, 0);
    layout->addWidget(v2, 0, 1);
    layout->addWidget(v3, 0, 2);
    layout->addWidget(v4, 0, 3);
    layout->addWidget(v5, 0, 4);

    setLayout(layout);

    resize(1280, 960);

//    connect(IOServer::instance(), &IOServer::configurationLoaded, this, &VoltmeterWidget::configurationChanged);
    configurationChanged();
}

void VoltmeterWidget::configurationChanged()
{
    auto server = IOServer::instance();
    if (!server)
        return;

    auto nodes = server->getNodes();
    v1->setDataSourceNode(findNodeByName(nodes, "PV1"));
    v2->setDataSourceNode(findNodeByName(nodes, "PV2"));
    v3->setDataSourceNode(findNodeByName(nodes, "PV3"));
    v4->setDataSourceNode(findNodeByName(nodes, "PV4"));
    v5->setDataSourceNode(findNodeByName(nodes, "PV6"));
}

INode *VoltmeterWidget::findNodeByName(QList<INode *> &container, const QString &nodeName) const
{
    for (auto node : container) {
        if (node->name() == nodeName) {
            return node;
        }
    }
    return nullptr;
}
