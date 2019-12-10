#include "AFCWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "Devices/Device.h"

AFCWidget::AFCWidget(QWidget *parent) : QWidget(parent)
{
    v1 = new Device(this);
    v2 = new Device(this);
    v3 = new Device(this);

    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(2);
    layout->setMargin(5);

    layout->addWidget(v1, 0, 0);
    layout->addWidget(v2, 0, 1);
    layout->addWidget(v3, 0, 2);

    setLayout(layout);

    resize(1280, 960);

//    connect(IOServer::instance(), &IOServer::configurationLoaded, this, &AFCWidget::configurationChanged);
    configurationChanged();
}

void AFCWidget::setViewForManualTestingWidget()
{
    v1->setHidden(true);
    v2->setHidden(true);

    v3->setMinimumHeight(400);
    v3->setMinimumWidth(250);
}

void AFCWidget::configurationChanged()
{
    auto server = IOServer::instance();
    if (!server)
        return;

    auto nodes = server->getNodes();
    v1->setDataSourceNode(findNodeByName(nodes, "UZ1"));
    v2->setDataSourceNode(findNodeByName(nodes, "UZ2"));
    v3->setDataSourceNode(findNodeByName(nodes, "UZ3"));
}

INode *AFCWidget::findNodeByName(QList<INode *> &container, const QString &nodeName) const
{
    for (auto node : container) {
        if (node->name() == nodeName) {
            return node;
        }
    }
    return nullptr;
}
