#include "MKTNWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "Devices/Device.h"

MKTNWidget::MKTNWidget(QWidget *parent, const QString &nodeName)
    : QWidget(parent)
{
    v1 = new Device(this);
    m_nodeName = nodeName;

    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(2);
    layout->setMargin(5);

    layout->addWidget(v1, 0, 0);


    setLayout(layout);

    resize(1280, 960);

    configurationChanged();

}

void MKTNWidget::configurationChanged()
{
    auto server = IOServer::instance();
    if (!server)
        return;

    auto nodes = server->getNodes();

#ifdef SAP_KSU
    v1->setDataSourceNode(findNodeByName(nodes, "PV5"));
#endif
#ifdef SAP_SU
    v1->setDataSourceNode(findNodeByName(nodes, "MKTN"));
#endif
#ifdef MECHOS
    v1->setDataSourceNode(findNodeByName(nodes, m_nodeName));
#endif

}

INode *MKTNWidget::findNodeByName(QList<INode *> &container, const QString &nodeName) const
{
    for (auto node : container) {
        if (node->name() == nodeName) {
            return node;
        }
    }
    return nullptr;
}
