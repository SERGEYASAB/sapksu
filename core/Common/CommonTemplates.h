#ifndef COMMONTEMPLATES_H
#define COMMONTEMPLATES_H

#include <QLineEdit>
#include <QVariant>

#include "FKValidatorSpinBox/FKInt64Validator.h"
#include "FKValidatorSpinBox/FKUInt64Validator.h"
#include "FKValidatorSpinBox/FKCP866StringValidator.h"

/// raw cast
namespace Sap
{
    template<typename To, typename From>
    struct raw_cast_t
    {
        union
        {
            From from;
            To to;
        };
    };

    template<typename To, typename From>
    To raw_cast(From raw_data)
    {
        raw_cast_t<To, From> raw_t;
        raw_t.from = raw_data;
        return raw_t.to;
    }
}


/// validators
namespace Sap {
    template <typename T>
    void setEditorValidator_impl(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue, std::true_type) {
        FKInt64Validator *validator = new FKInt64Validator(std::numeric_limits<T>::min(),
                                                           std::numeric_limits<T>::max(),
                                                           lineEdit);
        validator->setHexValidation(true);
        if (useMinMax && minValue.isValid() && !minValue.isNull() && minValue.canConvert<T>()) {
            validator->setBottom(minValue.value<T>());
        }

        if (useMinMax && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<T>()) {
            validator->setTop(maxValue.value<T>());
        }
        lineEdit->setValidator(validator);
    }

    template <typename T>
    void setEditorValidator_impl(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue, std::false_type) {
        FKUInt64Validator *validator = new FKUInt64Validator(std::numeric_limits<T>::min(),
                                                             std::numeric_limits<T>::max(),
                                                             lineEdit);
        validator->setHexValidation(true);
        if (useMinMax && minValue.isValid() && !minValue.isNull() && minValue.canConvert<T>()) {
            validator->setBottom(minValue.value<T>());
        }

        if (useMinMax && maxValue.isValid() && !maxValue.isNull() && maxValue.canConvert<T>()) {
            validator->setTop(maxValue.value<T>());
        }
        lineEdit->setValidator(validator);
    }


    template <typename T>
    void setEditorValidator(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue) {
        setEditorValidator_impl<T>(lineEdit, useMinMax, minValue, maxValue, typename std::integral_constant<bool, std::is_signed<T>::value>());
    }

    template <typename T>
    void setEditorValidator(QLineEdit *const lineEdit) {
        setEditorValidator<T>(lineEdit, false, QVariant(), QVariant());
    }

    void setFloatEditorValidator(QLineEdit *const lineEdit, uint precision = 4);
    void setFloatEditorValidator(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue, uint precision = 4);

    template <>
    void setEditorValidator<float>(QLineEdit *const lineEdit);
    template <>
    void setEditorValidator<float>(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue);


    void setDoubleEditorValidator(QLineEdit *const lineEdit, uint precision = 4);
    void setDoubleEditorValidator(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue, uint precision = 4);

    template <>
    void setEditorValidator<double>(QLineEdit *const lineEdit);
    template <>
    void setEditorValidator<double>(QLineEdit *const lineEdit, bool useMinMax, const QVariant &minValue, const QVariant &maxValue);
};

#endif // COMMONTEMPLATES_H
