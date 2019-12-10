#ifndef DOMITEM_H
#define DOMITEM_H

#include "Common/PrefixHeader.h"

class DomItem
{

public:
    DomItem(QObject *ptrNode, QDomNode &domNode, int row, DomItem *parent = Q_NULLPTR);
    ~DomItem();

    DomItem *child(int childNumber);
    DomItem *parent();

    QObject* ptrNode() const;
    QDomNode domNode() const;
    int row();

    int childCountWithoutIVariables();

private:
    QObject *m_ptrNode;
    QDomNode m_domNode;

    int m_row;
    DomItem *m_parent;
    QList<DomItem *> m_children;
};

#endif // DOMITEM_H
