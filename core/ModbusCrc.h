#ifndef MODBUSCRC_H
#define MODBUSCRC_H

#include <QObject>

class ModbusCrc
{

public:
    static ModbusCrc *getInstance();

    QByteArray calcCrc(const QByteArray &data, const quint16 &offset, const quint16 &length);
    bool exactMatch(const QByteArray &data);

private:
    ModbusCrc();
    ~ModbusCrc() {}

    QByteArray hi;
    QByteArray lo;
    QByteArray crc;

    Q_DISABLE_COPY(ModbusCrc)
};

#endif // MODBUSCRC_H
