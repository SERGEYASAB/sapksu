#ifndef ENERGOMONITOR12INTEGER20FRACTIONDEVICEVARIABLEVALUE_H
#define ENERGOMONITOR12INTEGER20FRACTIONDEVICEVARIABLEVALUE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/DeviceVariableValueBase.h"

class EnergoMonitor12Integer20FractionDeviceVariableValue : DeviceVariableValueBase<double>
{
public:
    EnergoMonitor12Integer20FractionDeviceVariableValue(IVariable &variable);

    // IDeviceVariableValue interface
public:
    bool setValueBytes(const QByteArray &bytes) override;
    QByteArray bytesFromValue(const QVariant &) const override;
    QByteArray currentValueBytes() const override;
    quint8 bytesCount() const override;
};

#endif // ENERGOMONITOR12INTEGER20FRACTIONDEVICEVARIABLEVALUE_H
