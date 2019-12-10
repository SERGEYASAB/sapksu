#include "DomItem.h"

#include "IOServer/nsConnection/IConnection.h"
#include "IOServer/nsNode/INode.h"

DomItem::DomItem(QObject *ptrNode, QDomNode &domNode, int row, DomItem *parent)
{
    m_ptrNode = ptrNode;
    m_domNode = domNode;
    m_row = row;
    m_parent = parent;

    if (parent) {
        parent->m_children.append(this);
    }
}

DomItem::~DomItem()
{
    auto it = m_children.begin();
    for (; it != m_children.end(); ++it)
        delete *it;

    m_parent->m_children.removeOne(this);
}

QObject *DomItem::ptrNode() const
{
    return m_ptrNode;
}

QDomNode DomItem::domNode() const
{
    return m_domNode;
}

DomItem *DomItem::parent()
{
    return m_parent;
}

DomItem *DomItem::child(int childNumber)
{
    if (childNumber >= 0 && childNumber < m_children.count()) {

        if (m_children[childNumber])
            return m_children[childNumber];
    }

    return nullptr;
}

int DomItem::row()
{
    return m_row;
}

int DomItem::childCountWithoutIVariables()
{
    int count = 0;

    for (auto childItem : m_children) {

        if (dynamic_cast<IConnection *>(childItem->ptrNode()) != nullptr) {
            count++;
        } else if (dynamic_cast<INode *>(childItem->ptrNode()) != nullptr) {
            count++;
        }
    }

    return count;
}
