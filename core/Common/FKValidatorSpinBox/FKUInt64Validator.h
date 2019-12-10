#ifndef FKUINT64VALIDATOR_H
#define FKUINT64VALIDATOR_H

#include <QValidator>

class FKUInt64Validator : public QValidator
{
    Q_OBJECT
    Q_PROPERTY(quint64 bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(quint64 top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(bool hexValidation READ hexValidation WRITE setHexValidation NOTIFY hexValidationChanged)

public:
    explicit FKUInt64Validator(QObject *parent = nullptr);
    FKUInt64Validator(quint64 bottom, quint64 top, QObject *parent = nullptr);
    ~FKUInt64Validator() {}

    State validate(QString &input, int &) const override;
    void fixup(QString &input) const override;

    inline quint64 bottom() const { return b; }
    inline quint64 top() const { return t; }

    inline void setBottom(quint64 bottom) { setRange(bottom, top()); }
    inline void setTop(quint64 top) { setRange(bottom(), top); }
    virtual void setRange(quint64 bottom, quint64 top);

    inline bool hexValidation() const { return m_hexValidation; }
    void setHexValidation(bool value);

signals:
    void bottomChanged(quint64 bottom);
    void topChanged(quint64 top);
    void hexValidationChanged(bool hexValidation);

private:
    Q_DISABLE_COPY(FKUInt64Validator)

    bool m_hexValidation = false;
    quint64 b;
    quint64 t;
};

#endif // FKUINT64VALIDATOR_H
