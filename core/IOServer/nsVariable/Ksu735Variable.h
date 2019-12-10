#ifndef KSU735VARIABLE_H
#define KSU735VARIABLE_H

#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/Ksu735DeviceVariableValue.h"

class Ksu735DeviceVariableValue;

class Ksu735Variable : public IVariable
{
    Q_OBJECT

    Q_PROPERTY(quint32 address READ address WRITE setAddress)
    Q_PROPERTY(SapKsu::Ksu735ParametrFormat format READ format WRITE setFormat)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
//    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue)
//    Q_PROPERTY(double minValue READ minValue WRITE setMinValue)
//    Q_PROPERTY(float divisionPrice READ divisionPrice WRITE setDivisionPrice)

public:
    explicit Ksu735Variable(QObject *parent = Q_NULLPTR);
    explicit Ksu735Variable(QDomElement *node, QObject *parent = Q_NULLPTR);
    ~Ksu735Variable();

    //Для 0х20 функции здесь хранится адрес
    //Для 0х10 - порядковый номер бита в возвращаемой маске
    quint32 address() const;
    void setAddress(quint32 address);


    inline SapKsu::Ksu735ParametrFormat format() const { return _format; }

    /*!
     * \brief setFormat устанавливает формат значения сервера
     * \param format формат значения сервера
     * \warning установка формата может изменить стратегию обработки значения с девайса
     */
    void setFormat(const SapKsu::Ksu735ParametrFormat &format);

    inline Ksu735DeviceVariableValue::ConversionType valueConversionType() const
    { return _variableValue->conversionType(); }
    inline void setValueConversionType(Ksu735DeviceVariableValue::ConversionType conversionType)
    { _variableValue->setConversionType(conversionType); }

signals:
    void addressChanged(quint8 address);

protected:
    IDeviceVariableValue *deviceVariableValue() const override;

    QByteArray convertByteOrder(const QByteArray &bytes) const override;

private:
    IVariable::StrategyType strategyTypeFromFormat(SapKsu::Ksu735ParametrFormat format);
    IVariable::DataType serverDataTypeFromFormat(SapKsu::Ksu735ParametrFormat format);

private:
    friend class Ksu735Node;

    Ksu735DeviceVariableValue *_variableValue = nullptr;
    quint32 _address = 0;
    SapKsu::Ksu735ParametrFormat _format = SapKsu::XXXX;

//    double _minValue;
//    double _maxValue;
//    float _divisionPrice;

    Q_DISABLE_COPY(Ksu735Variable)
};

#endif // KSU735VARIABLE_H
