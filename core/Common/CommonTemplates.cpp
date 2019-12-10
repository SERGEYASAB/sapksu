#include "CommonTemplates.h"

void Sap::setFloatEditorValidator(QLineEdit *const lineEdit, uint precision)
{
    Sap::setFloatEditorValidator(lineEdit, false, QVariant(), QVariant(), precision);
}

void Sap::setFloatEditorValidator(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue, uint precision)
{
    typedef float T;

    QLocale c(QLocale::C);
    c.setNumberOptions(QLocale::RejectGroupSeparator);

    QDoubleValidator *validator = new QDoubleValidator(std::numeric_limits<T>::lowest(),
                                                       std::numeric_limits<T>::max(),
                                                       precision,
                                                       lineEdit);
    validator->setNotation(QDoubleValidator::StandardNotation);
    validator->setLocale(c);

    if (useMinMax && minValue.isValid() && !minValue.isNull() && minValue.canConvert<T>()) {
        validator->setBottom(minValue.value<T>());
    }

    if (useMinMax && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<T>()) {
        validator->setTop(maxValue.value<T>());
    }
    lineEdit->setValidator(validator);
}

void Sap::setDoubleEditorValidator(QLineEdit *const lineEdit, uint precision)
{
    Sap::setDoubleEditorValidator(lineEdit, false, QVariant(), QVariant(), precision);
}

void Sap::setDoubleEditorValidator(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue, uint precision)
{
    typedef double T;

    QLocale c(QLocale::C);
    c.setNumberOptions(QLocale::RejectGroupSeparator);

    QDoubleValidator *validator = new QDoubleValidator(std::numeric_limits<T>::lowest(),
                                                       std::numeric_limits<T>::max(),
                                                       precision,
                                                       lineEdit);
    validator->setNotation(QDoubleValidator::StandardNotation);
    validator->setLocale(c);

    if (useMinMax && minValue.isValid() && !minValue.isNull() && minValue.canConvert<T>()) {
        validator->setBottom(minValue.value<T>());
    }

    if (useMinMax && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<T>()) {
        validator->setTop(maxValue.value<T>());
    }
    lineEdit->setValidator(validator);
}

template<>
void Sap::setEditorValidator<float>(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue)
{
    setFloatEditorValidator(lineEdit, useMinMax, minValue, maxValue);
}

template<>
void Sap::setEditorValidator<double>(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue)
{
    setDoubleEditorValidator(lineEdit, useMinMax, minValue, maxValue);
}

template<>
void Sap::setEditorValidator<float>(QLineEdit *const lineEdit)
{
    Sap::setEditorValidator<float>(lineEdit, false, QVariant(), QVariant());
}

template<>
void Sap::setEditorValidator<double>(QLineEdit *const lineEdit)
{
    Sap::setEditorValidator<double>(lineEdit, false, QVariant(), QVariant());
}


