#ifndef OPERATORCLICKREPORT_H
#define OPERATORCLICKREPORT_H

#include "Common/PrefixHeader.h"

class OperatorClickReport
{
public:

    static OperatorClickReport *instance();

    void write(const QString &message);

private:

    OperatorClickReport(OperatorClickReport const&) = delete;             // Copy construct
    OperatorClickReport(OperatorClickReport&&) = delete;                  // Move construct
    OperatorClickReport& operator=(OperatorClickReport const&) = delete;  // Copy assign
    OperatorClickReport& operator=(OperatorClickReport &&) = delete;      // Move assign

protected:
    explicit OperatorClickReport();
    virtual ~OperatorClickReport();
};

#endif // OPERATORCLICKREPORT_H
