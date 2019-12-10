#ifndef KSUMODBUSPARAMERTDOMITEM_H
#define KSUMODBUSPARAMERTDOMITEM_H

#include "Common/PrefixHeader.h"

class KSUModbusParamertDomItem
{
public:
    KSUModbusParamertDomItem(QDomElement &node) : m_domElement(node) {}

    QDomElement domElement() const { return m_domElement; }

private:
    QDomElement m_domElement;
};

#endif // KSUMODBUSPARAMERTDOMITEM_H
