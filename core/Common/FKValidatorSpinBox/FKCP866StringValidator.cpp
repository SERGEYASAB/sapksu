#include "FKCP866StringValidator.h"

static QString patternString = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмноп░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀рстуфхцчшщъыьэюяЁёЄєЇїЎў°∙·√№¤■ ";

FKCP866StringValidator::FKCP866StringValidator(QObject *parent) :
    QValidator(parent),
    m_length(-1)
{

}

FKCP866StringValidator::FKCP866StringValidator(int length, QObject *parent) :
    QValidator(parent),
    m_length(length)
{

}

void FKCP866StringValidator::setLength(int length)
{
    if (length <= 0)
        length = -1;

    if (m_length != length) {
        m_length = length;
        emit lengthChanged(m_length);
        emit changed();
    }
}

QValidator::State FKCP866StringValidator::validate(QString &input, int &) const
{
    QByteArray buff;

    if (!validateChars(input, &buff))
        return Invalid;

    if (m_length != -1 && input.size() > m_length)
        return Invalid;

    return Acceptable;
}

void FKCP866StringValidator::fixup(QString &input) const
{
    QByteArray buff;

    if (!validateChars(input, &buff)) {
        return;
    }

    if (m_length != -1 && input.size() > m_length) {
        input.resize(m_length);
    }
}

bool FKCP866StringValidator::validateChars(const QString &str, QByteArray *buff) const
{
    buff->clear();
    buff->reserve(str.length());

    for (int i = 0; i < str.length(); ++i) {
        QChar c = str.at(i);

        if (patternString.contains(c, Qt::CaseSensitive)) {
            buff->append(QString(c));
        } else {
            return false;
        }
    }

    return true;
}
