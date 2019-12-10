#include "Ksu735Variable.h"

//#include "IVariableValue/IVariableValue.h"
#include "VariableValue/DeviceVariableValue/Ksu735DeviceVariableValue.h"
#include "VariableValue/VariableValueStrategy/NumberVariableValueStrategy.h"

Ksu735Variable::Ksu735Variable(QObject *parent)
    : Ksu735Variable(nullptr, parent)
{
}

Ksu735Variable::Ksu735Variable(QDomElement *node, QObject *parent)
    : IVariable(node, parent), _variableValue(new Ksu735DeviceVariableValue(*this)), _address(0)
{
    _variableValue->setConversionType(Ksu735DeviceVariableValue::ConversionType::Bcd);

    //    _address = 0;
    //    _minValue = std::numeric_limits<double>::min();
    //    _maxValue = std::numeric_limits<double>::max();

    //Дабы не прописывать в конфиге, заполняем здесь
    //В имеющемся у меня унифицированном протоколе
    //Есть только беззнаковые двухбайтовые регистры в big-endian
}

Ksu735Variable::~Ksu735Variable()
{
    delete _variableValue;
}

IDeviceVariableValue *Ksu735Variable::deviceVariableValue() const
{
    return _variableValue;
}

quint32 Ksu735Variable::address() const
{
    return _address;
}

void Ksu735Variable::setAddress(quint32 address)
{
    if (_address == address)
        return;
    _address = address;
    emit addressChanged(_address); //Не делай лишние сигналы без необходимости
}

QByteArray Ksu735Variable::convertByteOrder(const QByteArray &bytes) const
{
    QByteArray result(bytes);

    int size = bytes.size();
    for (int i = 0; i < size; i++) {
        result[i] = bytes[size - i - 1];
    }
    return result;
}

IVariable::StrategyType Ksu735Variable::strategyTypeFromFormat(SapKsu::Ksu735ParametrFormat format)
{
    switch (format) {
        case SapKsu::XXX_X:
        case SapKsu::XX_XX:
        case SapKsu::X_XXX:
        case SapKsu::_XXXX:
            return IVariable::NumberStrategy;
            break;

        case SapKsu::XXXX:
        case SapKsu::XX1XX:
        case SapKsu::BIN:
        case SapKsu::CCCC:
        default:
            return IVariable::NoneStrategy;
            break;
    }
}

IVariable::DataType Ksu735Variable::serverDataTypeFromFormat(SapKsu::Ksu735ParametrFormat format)
{
    switch (format) {
        case SapKsu::XXX_X:
        case SapKsu::XX_XX:
        case SapKsu::X_XXX:
        case SapKsu::_XXXX:
            return IVariable::Float32;
            break;


        case SapKsu::XXXX:
        case SapKsu::XX1XX:
        case SapKsu::BIN:
        case SapKsu::CCCC:
        default:
            return IVariable::UInt16;
            break;
    }
}

void Ksu735Variable::setFormat(const SapKsu::Ksu735ParametrFormat &format)
{
    IVariable::StrategyType formatStrategyType = strategyTypeFromFormat(format);
    IVariable::DataType formatDataType = serverDataTypeFromFormat(format);

    if (_format != format || serverValueDataType() != formatDataType) {
        setServerValueDataType(formatDataType);
    }

    if (_format == format && valueStrategyType() == formatStrategyType && valueStrategy())
        return;

    _format = format;
    setValueStrategyType(formatStrategyType);

    if (_format == SapKsu::BIN || _format == SapKsu::CCCC) {
        _variableValue->setConversionType(Ksu735DeviceVariableValue::ConversionType::Bin);
    } else {
        _variableValue->setConversionType(Ksu735DeviceVariableValue::ConversionType::Bcd);
    }

    if (_format >= SapKsu::XXX_X && format <= SapKsu::_XXXX) {
        NumberVariableValueStrategy *strategy = dynamic_cast<NumberVariableValueStrategy *>(valueStrategy());
        if (!strategy) return;

        strategy->setCoefficientB(0.0);
        strategy->setCoefficientK(1.0);

        switch (_format) {
            case SapKsu::XXX_X:
                strategy->setCoefficientK(0.1);
                setPrecision(1);
                break;

            case SapKsu::XX_XX:
                strategy->setCoefficientK(0.01);
                setPrecision(2);
                break;

            case SapKsu::X_XXX:
                strategy->setCoefficientK(0.001);
                setPrecision(3);
                break;

            case SapKsu::_XXXX:
                strategy->setCoefficientK(0.0001);
                setPrecision(4);
                break;

            default:
                break;
        }
    }
}

//double Ksu735Variable::maxValue() const
//{
//    return _maxValue;
//}

//void Ksu735Variable::setMaxValue(double maxValue)
//{
//    _maxValue = maxValue;
//}

//double Ksu735Variable::minValue() const
//{
//    return _minValue;
//}

//void Ksu735Variable::setMinValue(double minValue)
//{
//    _minValue = minValue;
//}

//float Ksu735Variable::divisionPrice() const
//{
//    return _divisionPrice;
//}

//void Ksu735Variable::setDivisionPrice(float divisionPrice)
//{
//    _divisionPrice = divisionPrice;
//}

//QByteArray Ksu735Variable::convertVariantToBytes(const QVariant &value) const
//{
//    if (!value.canConvert<quint16>()) {
//        return QByteArray();
//    }

//    quint16 writingValue = value.value<quint16>();

//    quint16 val = QString::number(writingValue).toUShort(0, 16);
//    QByteArray bytes;

//    for (qint8 i = 0, offset = 0; i < dataSize(); i++, offset += 8) {
//        bytes.append(val >> offset);
//    }
//    return bytes;
//}

//void Ksu735Variable::convertBytesToVariant(const QByteArray &bytes)
//{
//    quint16 val = 0;

//    //По документам там всегда Big endian
//    for (qint8 pos = dataSize() - 1, offset = pos * 8; pos >= 0; pos--, offset -= 8) {
//        val |= quint8(bytes[pos]) << offset;
//    }

//    //Чисел с плавающей точкой, а так же отрицательных чисел в этом протоколе нет.
//    //Целые числа при получении(отправке) домножаются(делятся) на цену деления шкалы.
//    //Переводим из BCD, умножаем на цену деления, переводим в дабл
//    setValue(QString::number(val, 16).toUInt(0, 10));
//}

