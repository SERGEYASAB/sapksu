#include "DirectLogicWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "Devices/Device.h"

DirectLogicWidget::DirectLogicWidget(QWidget *parent) : QWidget(parent)
{
    a15 = new Device(this);
    a16 = new Device(this);
    a17 = new Device(this);
    a18 = new Device(this);
    a19 = new Device(this);
    a22 = new Device(this);
    a23 = new Device(this);
    a24 = new Device(this);
    a25 = new Device(this);
    a26 = new Device(this);
    a27 = new Device(this);
    a28 = new Device(this);
    a29 = new Device(this);

    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(2);
    layout->setMargin(5);

    layout->addWidget(a15, 0, 0);
    layout->addWidget(a16, 0, 1);
    layout->addWidget(a17, 0, 2);
    layout->addWidget(a18, 0, 3);
    layout->addWidget(a19, 0, 4);

    layout->addWidget(a22, 1, 0);
    layout->addWidget(a23, 1, 1);
    layout->addWidget(a24, 1, 2);
    layout->addWidget(a25, 1, 3);
    layout->addWidget(a26, 1, 4);
    layout->addWidget(a27, 1, 5);
    layout->addWidget(a28, 1, 6);
    layout->addWidget(a29, 1, 7);

//    QLabel *koyoLogotype = new QLabel();
//    QPixmap pix;
//    pix.load(":/Images/direclgc_koyo.png");
//    koyoLogotype->setPixmap(pix);
//    koyoLogotype->show();

    //layout->addWidget(koyoLogotype, 0, 5, 1, 8);

    setLayout(layout);

    resize(1280, 960);

//    connect(IOServer::instance(), &IOServer::configurationLoaded, this, &DirectLogicWidget::configurationChanged);
    configurationChanged();
}

void DirectLogicWidget::configurationChanged()
{
    auto server = IOServer::instance();
    if (!server)
        return;

    auto nodes = server->getNodes();
    a15->setDataSourceNode(findNodeByName(nodes, "R0A15"));
    a16->setDataSourceNode(findNodeByName(nodes, "R0A16"));
    a17->setDataSourceNode(findNodeByName(nodes, "R0A17"));
    a18->setDataSourceNode(findNodeByName(nodes, "R0A18"));
    a19->setDataSourceNode(findNodeByName(nodes, "R0A19"));
    a22->setDataSourceNode(findNodeByName(nodes, "R1A22"));
    a23->setDataSourceNode(findNodeByName(nodes, "R1A23"));
    a24->setDataSourceNode(findNodeByName(nodes, "R1A24"));
    a25->setDataSourceNode(findNodeByName(nodes, "R1A25"));
    a26->setDataSourceNode(findNodeByName(nodes, "R1A26"));
    a27->setDataSourceNode(findNodeByName(nodes, "R1A27"));
    a28->setDataSourceNode(findNodeByName(nodes, "R1A28"));
    a29->setDataSourceNode(findNodeByName(nodes, "R1A29"));
}

INode *DirectLogicWidget::findNodeByName(QList<INode *> &container, const QString &nodeName) const
{
    for (auto node : container) {
        if (node->name() == nodeName) {
            return node;
        }
    }
    return nullptr;
}


