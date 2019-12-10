#ifndef COMMONENUMS_H
#define COMMONENUMS_H

#include <QtCore/qobjectdefs.h>

class SapKsu {
    Q_GADGET

public:
    enum KSUProtocolType {
        Ksu735 = 0,
        Modbus = 1
    };
    Q_ENUM(KSUProtocolType)

    enum Ksu735ParametrFormat {
        XXXX    = 0,
        XXX_X   = 1,
        XX_XX   = 2,
        X_XXX   = 3,
        _XXXX   = 4,
        XX1XX   = 5,
        BIN     = 6,
        CCCC    = 7
    };
    Q_ENUM(Ksu735ParametrFormat)

    enum FullTestingStatus {
        Unknown = 0,
        Ok = 1,
        NotSame = 2,
        Error = 3
    };

    Q_ENUM(FullTestingStatus)

    static int paramertValueDivisorForFormat(Ksu735ParametrFormat format) noexcept;
    static double roundingToPrecision(double value, int precision) noexcept;
    static QString outputToPrecision(double value, int precision) noexcept;

    static const int IVariableRole = 0x1000;

private:
    SapKsu() = delete;
    SapKsu(const SapKsu &other) = delete;
    SapKsu &operator=(const SapKsu &other) = delete;
};

#endif // COMMONENUMS_H
