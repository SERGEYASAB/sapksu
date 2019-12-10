#include "FKUInt64Validator.h"
#include "FKValidatorHelper.h"

FKUInt64Validator::FKUInt64Validator(QObject *parent) :
    QValidator(parent), b(std::numeric_limits<quint64>::min()), t(std::numeric_limits<quint64>::max())
{

}

FKUInt64Validator::FKUInt64Validator(quint64 bottom, quint64 top, QObject *parent) :
    QValidator(parent), b(bottom), t(top)
{

}

QValidator::State FKUInt64Validator::validate(QString &input, int &) const
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

    if (buff.size() == 1 && buff.at(0) == '+')
        return Intermediate;

    bool ok;
    qint64 entered = buff.toLongLong(&ok, 0);
    if (!ok)
        return Invalid;

    if (entered >= b && entered <= t) {
        input.toULongLong(&ok, 0);
        return ok ? Acceptable : Intermediate;
    }

    return (entered > t) ? Invalid : Intermediate;
}

void FKUInt64Validator::fixup(QString &input) const
{
    QByteArray buff;
    FKValidatorHelper::Notation notation = FKValidatorHelper::Notation::Dec;
    if (m_hexValidation)
        notation = FKValidatorHelper::Notation::Hex;

    if (!FKValidatorHelper::validateChars(input, notation, &buff)) {
        return;
    }

    bool ok;
    qint64 entered = buff.toULongLong(&ok, 0);
    if (ok)
        input = QString::number(entered, static_cast<int>(notation));
}

void FKUInt64Validator::setRange(quint64 bottom, quint64 top)
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

void FKUInt64Validator::setHexValidation(bool value)
{
    if (m_hexValidation != value) {
        m_hexValidation = value;
        emit hexValidationChanged(m_hexValidation);
        emit changed();
    }
}
