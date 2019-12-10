#ifndef ENERGOMONITOR16SIGNED32INTEGER16FRACTIONDEVICEVARIABLEVALUE_H
#define ENERGOMONITOR16SIGNED32INTEGER16FRACTIONDEVICEVARIABLEVALUE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/DeviceVariableValueBase.h"

class EnergoMonitor16Signed32Integer16FractionDeviceVariableValue : public DeviceVariableValueBase<double>
{
public:
    EnergoMonitor16Signed32Integer16FractionDeviceVariableValue(IVariable &variable);

    // IDeviceVariableValue interface
public:
    bool setValueBytes(const QByteArray &bytes) override;
    QByteArray bytesFromValue(const QVariant &) const override;
    QByteArray currentValueBytes() const override;
    quint8 bytesCount() const override;
};

#endif // ENERGOMONITOR16SIGNED32INTEGER16FRACTIONDEVICEVARIABLEVALUE_H
