#ifndef ENGINELOCALVARIABLE_H
#define ENGINELOCALVARIABLE_H

#include "Common/PrefixHeader.h"

class EngineLocalVariable : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QVariant value READ value WRITE writeValue)
    Q_PROPERTY(bool writable READ isWritable WRITE setWritable)

public:
    explicit EngineLocalVariable(QObject *parent = 0);
    ~EngineLocalVariable();

    QVariant value() const;
    void writeValue(const QVariant &value);

    QString name() const;
    void setName(const QString &name);

    bool isWritable() const;
    void setWritable(bool writable);

    QString description() const;
    void setDescription(const QString &description);

private:
    QString m_name;
    QString m_description;
    QVariant m_value;
    bool m_writable;

signals:
    void valueChanged(EngineLocalVariable *);
};

#endif // ENGINELOCALVARIABLE_H
