#ifndef ENERGOMONITORSIGNED3INTEGER12FRACTIONDEVICEVARIABLEVALUE_H
#define ENERGOMONITORSIGNED3INTEGER12FRACTIONDEVICEVARIABLEVALUE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/DeviceVariableValueBase.h"

class EnergoMonitorSigned3Integer12FractionDeviceVariableValue : DeviceVariableValueBase<float>
{
public:
    EnergoMonitorSigned3Integer12FractionDeviceVariableValue(IVariable &variable);

    // IDeviceVariableValue interface
public:
    bool setValueBytes(const QByteArray &bytes) override;
    QByteArray bytesFromValue(const QVariant &) const override;
    QByteArray currentValueBytes() const override;
    quint8 bytesCount() const override;
};

#endif // ENERGOMONITORSIGNED3INTEGER12FRACTIONDEVICEVARIABLEVALUE_H
