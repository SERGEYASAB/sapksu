#ifndef FKVALIDATORHELPER_H
#define FKVALIDATORHELPER_H

#include "Common/PrefixHeader.h"

class FKValidatorHelper
{
public:
    enum Notation {
        Dec = 10,
        Hex = 16
    };

    static bool validateChars(const QString &str, Notation notation, QByteArray *buff);
    static bool validateDecChars(const QString &str, QByteArray *buff);
    static bool validateHexChars(const QString &str, QByteArray *buff);

private:
    FKValidatorHelper() = delete;
    FKValidatorHelper(const FKValidatorHelper &) = delete;
    FKValidatorHelper &operator=(const FKValidatorHelper &) = delete;
};

#endif // FKVALIDATORHELPER_H
