#ifndef INTERFERENCESENDER_H
#define INTERFERENCESENDER_H

#include "Common/PrefixHeader.h"

class InterferenceSender : public QObject
{
    Q_OBJECT

public:

    enum SenderType {
        NoSending           = 0x01,
        RandomSending       = 0x02,
        FixedSending        = 0x03,
        PresetSending       = 0x04,
    };
    Q_ENUM(SenderType)

    static InterferenceSender *instance();

    QString m_possibleCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

public:
    QByteArray setInterference(const QByteArray &data);

    SenderType getSenderType() const;
    void setSenderType(const SenderType &value);

    qint8 getRandomSendingBytesCount() const;
    void setRandomSendingBytesCount(const qint8 &value);

    qint8 getFixedSendingBytesCount() const;
    void setFixedSendingBytesCount(const qint8 &value);

    QByteArray getPrefixBytes() const;
    void setPrefixBytes(const QByteArray &value);

    QByteArray getPostfixBytes() const;
    void setPostfixBytes(const QByteArray &value);

private:
    explicit InterferenceSender();

    SenderType m_senderType = SenderType::NoSending;

    qint8 m_randomSendingBytesCount = 0;
    qint8 m_fixedSendingBytesCount = 0;

    QByteArray m_prefixBytes;
    QByteArray m_postfixBytes;
};

#endif // INTERFERENCESENDER_H
