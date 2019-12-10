#ifndef VARIABLEVALUETYPETRAITS_H
#define VARIABLEVALUETYPETRAITS_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsVariable/IVariable.h"

template <int _Tp>
struct VariableValueTypeTraits_impl {};

template <>
struct VariableValueTypeTraits_impl<IVariable::Boolean> {
    typedef bool type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::Int8> {
    typedef qint8 type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::UInt8> {
    typedef quint8 type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::Int16> {
    typedef qint16 type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::UInt16> {
    typedef quint16 type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::Int32> {
    typedef qint32 type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::UInt32> {
    typedef quint32 type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::Int64> {
    typedef qint64 type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::UInt64> {
    typedef quint64 type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::Float32> {
    typedef float type;
};

template <>
struct VariableValueTypeTraits_impl<IVariable::Float64> {
    typedef double type;
};

template <int _Tp>
struct VariableValueTypeTraits : VariableValueTypeTraits_impl<_Tp> {
    static constexpr int value = _Tp;
};

//namespace VariableValueTypeTraitsF {
//    constexpr IVariable::DataType getTypeFromVariable(int n) {
//        switch (n) {
//            case 0:
//                return IVariable::Boolean;
//                break;

//            case 10:
//                return IVariable::Int8;
//                break;

//            case 11:
//                return IVariable::Uint8;
//                break;

//            case 12:
//                return IVariable::Int16;
//                break;

//            case 14:
//                return IVariable::Uint32;
//                break;

//            case 15:
//                return IVariable::Uint32;
//                break;

//            case 16:
//                return IVariable::Int64;
//                break;

//            case 17:
//                return IVariable::Uint64;
//                break;

//            case 20:
//                return IVariable::Float32;
//                break;

//            case 21:
//                return IVariable::Float64;
//                break;

//            case 13:
//            default:
//                return IVariable::Uint16;
//                break;
//        }
//    }
//}

#endif // VARIABLEVALUETYPETRAITS_H
