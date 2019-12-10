#ifndef PARAMSREPORT_H
#define PARAMSREPORT_H

#include "Common/PrefixHeader.h"

class ParamsReport : public QObject
{
    Q_OBJECT

public:
    static ParamsReport *instance();

    enum ParamsReportFunction {
        Read       = 0x01,
        Write      = 0x02
    };
    Q_ENUM(ParamsReportFunction)

    void write(const ParamsReportFunction &function,
               const QByteArray &tx,
               const QByteArray &rx,
               const QString &aliasName,
               const QString &aliasValue,
               const QString &result
               );

private:

    ParamsReport(ParamsReport const&) = delete;             // Copy construct
    ParamsReport(ParamsReport&&) = delete;                  // Move construct
    ParamsReport& operator=(ParamsReport const&) = delete;  // Copy assign
    ParamsReport& operator=(ParamsReport &&) = delete;      // Move assign

protected:
    explicit ParamsReport(QObject *parent = nullptr);
};

#endif // PARAMSREPORT_H
