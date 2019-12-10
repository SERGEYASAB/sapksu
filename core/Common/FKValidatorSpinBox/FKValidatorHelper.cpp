#include "FKValidatorHelper.h"

bool FKValidatorHelper::validateChars(const QString &str, FKValidatorHelper::Notation notation, QByteArray *buff)
{
    if (notation == Notation::Dec) {
        return validateDecChars(str, buff);
    } else {
        return validateHexChars(str, buff);
    }
}

bool FKValidatorHelper::validateDecChars(const QString &str, QByteArray *buff)
{
    buff->clear();
    buff->reserve(str.length());

    bool lastWasDigit = false;

    for (int i = 0; i < str.length(); ++i) {
        char c = str.at(i).toLatin1();

        if (c >= '0' && c <= '9') {
            lastWasDigit = true;
        } else {
            switch (c) {
                case '+':
                case '-':
                    if (i != 0)
                        return false;
                    break;

                default:
                    return false;
            }
            lastWasDigit = false;
        }

        buff->append(c);
    }

    return true;
}

bool FKValidatorHelper::validateHexChars(const QString &str, QByteArray *buff)
{
    buff->clear();
    buff->reserve(str.length());

    bool lastWasDigit = false;
    bool lastWasZero = false;
    bool isHex = false;

    for (int i = 0; i < str.length(); ++i) {
        char c = str.at(i).toLatin1();
        char lc = toupper(c);

        if (c >= '0' && c <= '9') {
            lastWasZero = c == '0';
            lastWasDigit = true;
        } else if (lc >= 'A' && lc <= 'F') {
            if (!isHex)
                return false;

            lastWasDigit = false;
            lastWasZero = false;
        } else {
            switch (c) {
                case '+':
                case '-':
                    if (i != 0)
                        return false;
                    break;

                case 'x':
                    if (i == 1 && lastWasZero) {
                        isHex = true;
                    } else {
                        return false;
                    }
                    break;

                default:
                    return false;
            }
            lastWasDigit = false;
            lastWasZero = false;
        }

        buff->append(c);
    }

    return true;
}
