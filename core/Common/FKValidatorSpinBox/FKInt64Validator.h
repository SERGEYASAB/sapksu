#ifndef FKINT64VALIDATOR_H
#define FKINT64VALIDATOR_H

#include <QValidator>

class FKInt64Validator : public QValidator
{
    Q_OBJECT
    Q_PROPERTY(qint64 bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(qint64 top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(bool hexValidation READ hexValidation WRITE setHexValidation NOTIFY hexValidationChanged)

public:
    explicit FKInt64Validator(QObject *parent = nullptr);
    FKInt64Validator(qint64 bottom, qint64 top, QObject *parent = nullptr);
    ~FKInt64Validator() {}

    State validate(QString &input, int &) const override;
    void fixup(QString &input) const override;

    inline qint64 bottom() const { return b; }
    inline qint64 top() const { return t; }

    void setBottom(qint64 bottom) { setRange(bottom, top()); }
    void setTop(qint64 top)  { setRange(bottom(), top); }
    virtual void setRange(qint64 bottom, qint64 top);

    inline bool hexValidation() const { return m_hexValidation; }
    void setHexValidation(bool value);

signals:
    void bottomChanged(qint64 bottom);
    void topChanged(qint64 top);
    void hexValidationChanged(bool hexValidation);

private:
    Q_DISABLE_COPY(FKInt64Validator)

    bool m_hexValidation = false;
    qint64 b;
    qint64 t;
};

#endif // FKINT64VALIDATOR_H
