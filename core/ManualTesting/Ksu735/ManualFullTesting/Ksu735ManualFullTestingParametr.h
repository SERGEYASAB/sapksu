#ifndef KSU735MANUALFULLTESTINGPARAMETR_H
#define KSU735MANUALFULLTESTINGPARAMETR_H

#include "Common/PrefixHeader.h"

class Ksu735Variable;

struct Ksu735ManualFullTestingParametr {
    Ksu735ManualFullTestingParametr(Ksu735Variable *variable) noexcept : m_variable(variable) {}

    inline Ksu735Variable *variable() const noexcept { return m_variable; }

    bool enabled = false;
    QVariant writingValue;
    QVariant oldReadedValue;
    QVariant writedValue;
    QVariant readedValue;
    QString statusString;
    SapKsu::FullTestingStatus status = SapKsu::FullTestingStatus::Unknown;

    inline void clear();
    inline void clearWritingValue();

private:
    Ksu735Variable *m_variable;
};

inline void Ksu735ManualFullTestingParametr::clear()
{
    oldReadedValue.clear();
    writedValue.clear();
    readedValue.clear();
    statusString.clear();
    status = SapKsu::FullTestingStatus::Unknown;
}

inline void Ksu735ManualFullTestingParametr::clearWritingValue()
{
    writingValue.clear();
}

typedef QList<Ksu735ManualFullTestingParametr *> Ksu735ManualFullTestingParametrsList;

#endif // KSU735MANUALFULLTESTINGPARAMETR_H
