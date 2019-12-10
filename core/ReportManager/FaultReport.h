#ifndef FAULTREPORT_H
#define FAULTREPORT_H

#include "Common/PrefixHeader.h"

class FaultReport
{
public:

    static FaultReport *instance();

    void write(const QString &message);

private:

    FaultReport(FaultReport const&) = delete;             // Copy construct
    FaultReport(FaultReport&&) = delete;                  // Move construct
    FaultReport& operator=(FaultReport const&) = delete;  // Copy assign
    FaultReport& operator=(FaultReport &&) = delete;      // Move assign

protected:
    explicit FaultReport();
    virtual ~FaultReport();
};

#endif // FAULTREPORT_H
