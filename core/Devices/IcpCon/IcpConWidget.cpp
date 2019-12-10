#include "IcpConWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "Devices/Device.h"

IcpConWidget::IcpConWidget(QWidget *parent) : QWidget(parent)
{
    slot0 = new Device(this);
    slot1 = new Device(this);
    slot2 = new Device(this);
    slot3 = new Device(this);

    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(2);
    layout->setMargin(5);

    layout->addWidget(slot0, 0, 0);
    layout->addWidget(slot1, 0, 1);
    layout->addWidget(slot2, 0, 2);
    layout->addWidget(slot3, 0, 3);

    setLayout(layout);

    resize(1280, 960);
    configurationChanged();
}

void IcpConWidget::configurationChanged()
{
    auto server = IOServer::instance();
    if (!server)
        return;

    auto nodes = server->getNodes();
    slot0->setDataSourceNode(findNodeByName(nodes, "A7"));
    slot1->setDataSourceNode(findNodeByName(nodes, "A8"));
    slot2->setDataSourceNode(findNodeByName(nodes, "A9"));
    slot3->setDataSourceNode(findNodeByName(nodes, "A10"));
}

INode *IcpConWidget::findNodeByName(QList<INode *> &container, const QString &nodeName) const
{
    for (auto node : container) {
        if (node->name() == nodeName) {
            return node;
        }
    }
    return nullptr;
}


