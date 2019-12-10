#include "CircutorWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "Devices/Device.h"

CircutorWidget::CircutorWidget(bool attestation, QWidget *parent) : _attestation(attestation), QWidget(parent)
{
    v1 = new Device(this);
    v2 = new Device(this);
    v3 = new Device(this);

    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(2);
    layout->setMargin(5);

#ifdef SAP_KSU
    layout->addWidget(v1, 0, 0);
    layout->addWidget(v2, 0, 1);
#endif
#ifdef SAP_SU
    layout->addWidget(v1, 0, 0);
    layout->addWidget(v2, 0, 1);
#endif
#ifdef MECHOS
    layout->addWidget(v1, 0, 0);
    layout->addWidget(v2, 0, 1);
    layout->addWidget(v3, 0, 2);
#endif

    setLayout(layout);

    resize(1280, 960);
    configurationChanged();
}

void CircutorWidget::configurationChanged()
{
    auto server = IOServer::instance();
    if (!server)
        return;

    auto nodes = server->getNodes();

#ifdef SAP_KSU
    v1->setDataSourceNode(findNodeByName(nodes, "Circutor1"));
    v2->setDataSourceNode(findNodeByName(nodes, "Circutor2"));
#endif
#ifdef SAP_SU
    v1->setDataSourceNode(findNodeByName(nodes, "Circutor1"));
    v2->setDataSourceNode(findNodeByName(nodes, "Circutor2"));
#endif
#ifdef MECHOS
    v1->setDataSourceNode(findNodeByName(nodes, "Circutor1"));
    v2->setDataSourceNode(findNodeByName(nodes, "Circutor2"));
    v3->setDataSourceNode(findNodeByName(nodes, "Circutor3"));

    if (_attestation) {

        QList<int> hiddenRowsV1 = {0,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
        QList<int> hiddenRowsV2 = {0,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
        QList<int> hiddenRowsV3 = {0,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};

        foreach (auto i, hiddenRowsV1)
            v1->getView()->hideRow(i);
        foreach (auto i, hiddenRowsV2)
            v2->getView()->hideRow(i);
        foreach (auto i, hiddenRowsV3)
            v3->getView()->hideRow(i);
    }
#endif

}

void CircutorWidget::setInvisibleDevice(int deviceNumber, bool value)
{
    switch(deviceNumber)
    {
    case 1:
        v1->setHidden(value);
        break;
    case 2:
        v2->setHidden(value);
        break;
    case 3:
        v3->setHidden(value);
        break;
    }
    return;
}

INode *CircutorWidget::findNodeByName(QList<INode *> &container, const QString &nodeName) const
{
    for (auto node : container) {
        if (node->name() == nodeName) {
            return node;
        }
    }
    return nullptr;
}
