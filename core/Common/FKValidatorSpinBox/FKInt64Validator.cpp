#include "FKInt64Validator.h"
#include "FKValidatorHelper.h"

FKInt64Validator::FKInt64Validator(QObject *parent) :
    QValidator(parent), b(std::numeric_limits<qint64>::min()), t(std::numeric_limits<qint64>::max())
{

}

FKInt64Validator::FKInt64Validator(qint64 bottom, qint64 top, QObject *parent) :
    QValidator(parent), b(bottom), t(top)
{

}

QValidator::State FKInt64Validator::validate(QString &input, int &) const
{
    static const QByteArray hexStart = "0x";

    QByteArray buff;
    FKValidatorHelper::Notation notation = FKValidatorHelper::Notation::Dec;
    if (m_hexValidation)
        notation = FKValidatorHelper::Notation::Hex;

    if (!FKValidatorHelper::validateChars(input, notation, &buff)) {
        return Invalid;
    }

    if (buff.isEmpty())
        return Intermediate;

    if (m_hexValidation && buff == hexStart)
        return Intermediate;

    if (b >= 0 && buff.startsWith('-'))
        return Invalid;

    if (t < 0 && buff.startsWith('+'))
        return Invalid;

    if (buff.size() == 1 && (buff.at(0) == '+' || buff.at(0) == '-'))
        return Intermediate;

    bool ok;
    qint64 entered = buff.toLongLong(&ok, 0);
    if (!ok)
        return Invalid;

    if (entered >= b && entered <= t) {
        input.toLongLong(&ok, 0);
        return ok ? Acceptable : Intermediate;
    }

    if (entered >= 0) {
        return (entered > t && -entered < b) ? Invalid : Intermediate;
    } else {
        return (entered < b) ? Invalid : Intermediate;
    }
}

void FKInt64Validator::fixup(QString &input) const
{
    QByteArray buff;
    FKValidatorHelper::Notation notation = FKValidatorHelper::Notation::Dec;
    if (m_hexValidation)
        notation = FKValidatorHelper::Notation::Hex;

    if (!FKValidatorHelper::validateChars(input, notation, &buff)) {
        return;
    }

    bool ok;
    qint64 entered = buff.toLongLong(&ok, 0);
    if (ok)
        input = QString::number(entered, static_cast<int>(notation));
}

void FKInt64Validator::setRange(qint64 bottom, qint64 top)
{
    bool rangeChanged = false;
    if (b != bottom) {
        b = bottom;
        rangeChanged = true;
        emit bottomChanged(b);
    }

    if (t != top) {
        t = top;
        rangeChanged = true;
        emit topChanged(t);
    }

    if (rangeChanged)
        emit changed();
}

void FKInt64Validator::setHexValidation(bool value)
{
    if (m_hexValidation != value) {
        m_hexValidation = value;
        emit hexValidationChanged(m_hexValidation);
        emit changed();
    }
}
