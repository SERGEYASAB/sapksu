#include "Ksu735Crc.h"

Ksu735Crc *Ksu735Crc::getInstance()
{
    static Ksu735Crc singletone;
    return &singletone;
}

QByteArray Ksu735Crc::calcCrc(const QByteArray &data, quint16 offset, quint16 length)
{
    quint16 sum = 0;
    while (length--) {
        sum += quint8(data[offset++]);
    }
    return QByteArray().append(char(sum)).append(char(sum >> 8));
}

bool Ksu735Crc::exactMatch(const QByteArray &data)
{
    if (data.size() < 2)
        return false;

    return data.endsWith(calcCrc(data, 0, data.size() - 2));
}
