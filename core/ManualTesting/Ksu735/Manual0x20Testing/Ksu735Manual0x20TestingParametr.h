#ifndef KSU735MANUAL0X20TESTINGPARAMETR_H
#define KSU735MANUAL0X20TESTINGPARAMETR_H

#include "Common/PrefixHeader.h"

class Ksu735Variable;

struct Ksu735Manual0x20TestingParametr
{
    bool enabled = false;
    Ksu735Variable *variable = nullptr;
    QVariant writedValue;
    QVariant readedValue;
};

typedef QList<Ksu735Manual0x20TestingParametr *> Ksu735Manual0x20TestingParametrsList;

#endif // KSU735MANUAL0X20TESTINGPARAMETR_H
