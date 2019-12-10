#ifndef KSU0X20PARAMETR_H
#define KSU0X20PARAMETR_H

#include "Common/PrefixHeader.h"

class KSU0x20Parametr : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint8 address READ address WRITE setAddress)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString alias READ alias WRITE setAlias)
    Q_PROPERTY(bool writable READ writable WRITE setWritable)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(SapKsu::Ksu735ParametrFormat format READ format WRITE setFormat)
    Q_PROPERTY(quint16 minValue READ minValue WRITE setMinValue)
    Q_PROPERTY(quint16 maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(quint16 defaultValue READ defaultValue WRITE setDefaultValue)
    Q_PROPERTY(QString permissibleVariation READ permissibleVariation WRITE setPermissibleVariation)
    Q_PROPERTY(QString measureUnit READ measureUnit WRITE setMeasureUnit)

public:
    KSU0x20Parametr(QObject *parent = nullptr) noexcept;

    quint8 address() const noexcept;
    void setAddress(const quint8 &address) noexcept;

    QString name() const noexcept;
    void setName(const QString &name) noexcept;

    QString alias() const noexcept;
    void setAlias(const QString &alias) noexcept;

    bool writable() const noexcept;
    void setWritable(bool writable) noexcept;

    bool enabled() const noexcept;
    void setEnabled(bool enabled) noexcept;

    SapKsu::Ksu735ParametrFormat format() const noexcept;
    void setFormat(SapKsu::Ksu735ParametrFormat format) noexcept;

    quint16 minValue() const noexcept;
    void setMinValue(quint16 minValue) noexcept;

    quint16 maxValue() const noexcept;
    void setMaxValue(quint16 maxValue) noexcept;

    quint16 defaultValue() const noexcept;
    void setDefaultValue(quint16 defaultValue) noexcept;

    QString permissibleVariation() const noexcept;
    void setPermissibleVariation(const QString &permissibleVariation) noexcept;

    QString measureUnit() const;
    void setMeasureUnit(const QString &measureUnit);

private:
    quint8 m_address; // code
    QString m_name;
    QString m_alias;
    bool m_writable;
    bool m_enabled;
    SapKsu::Ksu735ParametrFormat m_format;
    quint16 m_minValue = 0;
    quint16 m_maxValue = 0;
    quint16 m_defaultValue = 0;
    QString m_permissibleVariation;
    QString m_measureUnit;
};

#endif // KSU0X20PARAMETR_H
