#ifndef KSU735NODEARCHIVE_H
#define KSU735NODEARCHIVE_H

#include "Common/PrefixHeader.h"

class Ksu735Node;

class Ksu735NodeArchive : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint8 stopReasonCode READ stopReasonCode)
    Q_PROPERTY(quint8 parametrNumber READ parametrNumber)
    Q_PROPERTY(quint8 parametrDeviceValue READ parametrDeviceValue)
    Q_PROPERTY(QDateTime recordDate READ recordDate)
    Q_PROPERTY(QString stopReasonName READ stopReasonName)
    Q_PROPERTY(QString parametrName READ parametrName)
    Q_PROPERTY(QVariant parametrValue READ parametrValue)

public:
    explicit Ksu735NodeArchive(Ksu735Node *node, const QByteArray &recordBytes, QObject *parent = nullptr);

    inline QDateTime recordDate() const { return m_recordDate; }
    inline QString stopReasonName() const { return m_stopReasonName; }
    inline QString parametrName() const { return m_parametrName; }
    inline QVariant parametrValue() const { return m_parametrValue; }
    inline bool isValid() const { return m_valid; }
    inline bool isEmpty() const { return m_empty; }

    inline quint8 stopReasonCode() const { return m_stopReasonCode; }
    inline quint8 parametrNumber() const { return m_parametrNumber; }
    inline quint8 parametrDeviceValue() const { return m_parametrDeviceValue; }

private:
    bool m_valid = false;
    bool m_empty = true;

    Ksu735Node *m_node = nullptr;
    QByteArray m_recordBytes;

    QDateTime m_recordDate;
    quint8 m_stopReasonCode = 0;
    quint8 m_parametrNumber = 0;
    quint16 m_parametrDeviceValue = 0;

    QString m_stopReasonName;
    QString m_parametrName;
    QVariant m_parametrValue;
};

#endif // KSU735NODEARCHIVE_H
