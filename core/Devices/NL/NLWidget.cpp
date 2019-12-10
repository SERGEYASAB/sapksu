#include "NLWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "Devices/Device.h"
#include "RizElecton08Widget.h"

NLWidget::NLWidget(QWidget *parent) : QWidget(parent)
{
    QWidget *tabWidget = new QWidget(this);

    nlRiz = new Device(this);

    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(2);
    layout->setMargin(5);
    layout->addWidget(nlRiz, 0, 0);

    tabWidget->setLayout(layout);

    RizElecton08Widget *el08Widget = new RizElecton08Widget(this);

    QTabWidget *rizTabWidget = new QTabWidget(this);
    rizTabWidget->insertTab(0, tabWidget, QIcon(), "Сопротивление изоляции");
    rizTabWidget->insertTab(1, el08Widget, QIcon(), "Сопротивление изоляции - Электон 08");
//    rizTabWidget->insertTab(2, tabWidget, QIcon(), "Сопротивление изоляции - Электон 09");
//    rizTabWidget->insertTab(3, tabWidget, QIcon(), "Сопротивление изоляции - Электон 10");
//    rizTabWidget->insertTab(4, tabWidget, QIcon(), "Сопротивление изоляции - ИРЗ");


    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    centralLayout->addWidget(rizTabWidget);

    setLayout(centralLayout);

    configurationChanged();
}

void NLWidget::configurationChanged()
{
    auto server = IOServer::instance();
    if (!server)
        return;

    auto nodes = server->getNodes();
    nlRiz->setDataSourceNode(findNodeByName(nodes, "NL16DO"));
}

INode *NLWidget::findNodeByName(QList<INode *> &container, const QString &nodeName) const
{
    for (auto node : container) {
        if (node->name() == nodeName) {
            return node;
        }
    }
    return nullptr;
}
