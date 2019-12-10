#include "InterferenceSender.h"

InterferenceSender *InterferenceSender::instance()
{
    static InterferenceSender singletone;
    return &singletone;
}

QByteArray InterferenceSender::setInterference(const QByteArray &data)
{
    QByteArray preparedData = data;

    switch (m_senderType) {

        case SenderType::NoSending: {
            preparedData = preparedData;
            break;
        }

        case SenderType::RandomSending: {

            const int randomStringLength = qrand() % m_randomSendingBytesCount;

            for(int i = 0; i < randomStringLength; i++) {
                int index = qrand() % m_possibleCharacters.length();
                QChar nextChar = m_possibleCharacters.at(index);
                preparedData.prepend(nextChar.toLatin1());
            }

            break;
        }

        case SenderType::FixedSending: {

            for(int i = 0; i < m_fixedSendingBytesCount; i++) {
                int index = qrand() % m_possibleCharacters.length();
                QChar nextChar = m_possibleCharacters.at(index);
                preparedData.prepend(nextChar.toLatin1());
            }

            break;
        }

        case SenderType::PresetSending: {
            preparedData.append(m_postfixBytes);
            preparedData.prepend(m_prefixBytes);
            break;
        }

        default: {
            preparedData = data;
            break;
        }
    }

    return preparedData;
}

InterferenceSender::InterferenceSender()
{

}

QByteArray InterferenceSender::getPostfixBytes() const
{
    return m_postfixBytes;
}

void InterferenceSender::setPostfixBytes(const QByteArray &value)
{
    m_postfixBytes = value;
}

QByteArray InterferenceSender::getPrefixBytes() const
{
    return m_prefixBytes;
}

void InterferenceSender::setPrefixBytes(const QByteArray &value)
{
    m_prefixBytes = value;
}

qint8 InterferenceSender::getFixedSendingBytesCount() const
{
    return m_fixedSendingBytesCount;
}

void InterferenceSender::setFixedSendingBytesCount(const qint8 &value)
{
    m_fixedSendingBytesCount = value;
}

qint8 InterferenceSender::getRandomSendingBytesCount() const
{
    return m_randomSendingBytesCount;
}

void InterferenceSender::setRandomSendingBytesCount(const qint8 &value)
{
    m_randomSendingBytesCount = value;
}

InterferenceSender::SenderType InterferenceSender::getSenderType() const
{
    return m_senderType;
}

void InterferenceSender::setSenderType(const SenderType &value)
{
    m_senderType = value;
}
