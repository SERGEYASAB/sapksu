#ifndef FKCP866STRINGVALIDATOR_H
#define FKCP866STRINGVALIDATOR_H

#include "Common/PrefixHeader.h"

class FKCP866StringValidator : public QValidator
{
    Q_OBJECT
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)

public:
    explicit FKCP866StringValidator(QObject *parent = nullptr);
    explicit FKCP866StringValidator(int length, QObject *parent = nullptr);
    ~FKCP866StringValidator() {}

    inline int length() const { return m_length; }
    void setLength(int length);

    // QValidator interface
public:
    State validate(QString &input, int &) const override;
    void fixup(QString &input) const override;

signals:
    void lengthChanged(int length);

private:
    bool validateChars(const QString &str, QByteArray *buff) const;

private:
    int m_length;
};

#endif // FKCP866STRINGVALIDATOR_H
