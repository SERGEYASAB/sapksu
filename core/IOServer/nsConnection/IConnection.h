#ifndef ICONNECTION_H
#define ICONNECTION_H

#include <QObject>
#include <QDomElement>
#include <QSerialPort>

class IDataTask;

class IConnection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(quint32 nextRequestDelay READ nextRequestDelay WRITE setNextRequestDelay)

public:
    explicit IConnection(QObject *parent = Q_NULLPTR);
    explicit IConnection(QDomElement &domElement, QObject *parent = Q_NULLPTR);
    virtual ~IConnection();

    // id
    QString id() const;
    void setId(const QString &id);

    // название
    QString name() const;
    void setName(const QString &name);

    // описание
    QString description() const;
    void setDescription(const QString &description);

    // конфигурация DOM
    QDomElement domElement();

    quint32 nextRequestDelay() const noexcept;
    void setNextRequestDelay(quint32 delay) noexcept;

    virtual void open() = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;

    // обработать задачу
    virtual QByteArray sendData(const QByteArray &request, quint32 timeout) = 0;

private:
    QString m_id;
    QString m_name;
    QString m_description;
    quint32 m_nextRequestDelay = 5;

protected:
    QDomElement m_domElement;
};

#endif // ICONNECTION_H
