#include "CommonEnums.h"

#include <cmath>
#include <QTextStream>

int SapKsu::paramertValueDivisorForFormat(SapKsu::Ksu735ParametrFormat format) noexcept
{
    switch (format) {
        case SapKsu::Ksu735ParametrFormat::XXX_X:
            return 10;
            break;

        case SapKsu::Ksu735ParametrFormat::XX_XX:
            return 100;
            break;

        case SapKsu::Ksu735ParametrFormat::X_XXX:
            return 1000;
            break;

        case SapKsu::Ksu735ParametrFormat::_XXXX:
            return 10000;

        case SapKsu::Ksu735ParametrFormat::XXXX:
        case SapKsu::Ksu735ParametrFormat::XX1XX:
        case SapKsu::Ksu735ParametrFormat::BIN:
        case SapKsu::Ksu735ParametrFormat::CCCC:
        default:
            return 1;
            break;
    }
}

double SapKsu::roundingToPrecision(double value, int precision) noexcept
{
    if (precision == -1)
        return value;

    if (value == 0.)
        return value;

    value *= pow(10, precision);
    value = round(value);
    value /= pow(10, precision);
}

QString SapKsu::outputToPrecision(double value, int precision) noexcept
{
    QString outputText;
    QTextStream stream(&outputText);
    stream.setNumberFlags(QTextStream::UppercaseDigits);
    if (precision == -1) {
        stream.setRealNumberNotation(QTextStream::ScientificNotation);
    } else {
        stream.setRealNumberNotation(QTextStream::FixedNotation);
        stream.setRealNumberPrecision(precision);
        if (precision != 0)
            stream.setNumberFlags(QTextStream::ForcePoint);
    }

    stream << value;
    stream.flush();

    return outputText;
}
