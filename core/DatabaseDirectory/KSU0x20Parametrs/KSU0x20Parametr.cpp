#include "KSU0x20Parametr.h"

KSU0x20Parametr::KSU0x20Parametr(QObject *parent) noexcept :
    QObject(parent),
    m_address(0),
    m_writable(false),
    m_enabled(false),
    m_format(SapKsu::XXXX),
    m_minValue(0),
    m_maxValue(9999),
    m_defaultValue(0)
{

}

quint8 KSU0x20Parametr::address() const noexcept
{
    return m_address;
}

void KSU0x20Parametr::setAddress(const quint8 &address) noexcept
{
    m_address = address;
}

QString KSU0x20Parametr::name() const noexcept
{
    return m_name;
}

void KSU0x20Parametr::setName(const QString &name) noexcept
{
    m_name = name;
}

QString KSU0x20Parametr::alias() const noexcept
{
    return m_alias;
}

void KSU0x20Parametr::setAlias(const QString &alias) noexcept
{
    m_alias = alias;
}

bool KSU0x20Parametr::writable() const noexcept
{
    return m_writable;
}

void KSU0x20Parametr::setWritable(bool writable) noexcept
{
    m_writable = writable;
}

bool KSU0x20Parametr::enabled() const noexcept
{
    return m_enabled;
}

void KSU0x20Parametr::setEnabled(bool enabled) noexcept
{
    m_enabled = enabled;
}

SapKsu::Ksu735ParametrFormat KSU0x20Parametr::format() const noexcept
{
    return m_format;
}

void KSU0x20Parametr::setFormat(SapKsu::Ksu735ParametrFormat format) noexcept
{
    m_format = format;
}

quint16 KSU0x20Parametr::minValue() const noexcept
{
    return m_minValue;
}

void KSU0x20Parametr::setMinValue(quint16 minValue) noexcept
{
    m_minValue = minValue;
}

quint16 KSU0x20Parametr::maxValue() const noexcept
{
    return m_maxValue;
}

void KSU0x20Parametr::setMaxValue(quint16 maxValue) noexcept
{
    m_maxValue = maxValue;
}

quint16 KSU0x20Parametr::defaultValue() const noexcept
{
    return m_defaultValue;
}

void KSU0x20Parametr::setDefaultValue(quint16 defaultValue) noexcept
{
    m_defaultValue = defaultValue;
}

QString KSU0x20Parametr::permissibleVariation() const noexcept
{
    return m_permissibleVariation;
}

void KSU0x20Parametr::setPermissibleVariation(const QString &permissibleVariation) noexcept
{
    m_permissibleVariation = permissibleVariation;
}

QString KSU0x20Parametr::measureUnit() const
{
    return m_measureUnit;
}

void KSU0x20Parametr::setMeasureUnit(const QString &measureUnit)
{
    m_measureUnit = measureUnit;
}

