#ifndef KSU735CRC_H
#define KSU735CRC_H

#include <QByteArray>

class Ksu735Crc final
{
public:
    static Ksu735Crc *getInstance();

    QByteArray calcCrc(const QByteArray &data, quint16 offset, quint16 length);
    bool exactMatch(const QByteArray &data);

private:
    Ksu735Crc() noexcept = default;
    ~Ksu735Crc() noexcept = default;

    Q_DISABLE_COPY(Ksu735Crc)
};

#endif // KSU735CRC_H
