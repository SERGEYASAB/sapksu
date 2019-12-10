#ifndef ENERGOFORMCRC16_H
#define ENERGOFORMCRC16_H

#include <QByteArray>

class EnergoFormCrc16 final
{
public:
    static QByteArray calcCrc(const QByteArray &data);
    static QByteArray calcCrc(const QByteArray &data, quint16 offset, quint16 length);
    static bool exactMatch(const QByteArray &data);

private:
    EnergoFormCrc16() noexcept = default;
    ~EnergoFormCrc16() noexcept = default;
};

#endif // ENERGOFORMCRC16_H
