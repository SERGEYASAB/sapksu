#ifndef IVARIABLE_H
#define IVARIABLE_H

#include "Common/PrefixHeader.h"
#include "VariableValue/ServerVariableValue.h"
#include "VariableValue/VariableValueStrategy/VariableValueStrategy.h"
#include "IOServer/NodeError/NodeError.h"

class INode;
class IVariableValue;
class IDeviceVariableValue;

class IVariable : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString alias READ alias WRITE setAlias)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(bool readable READ isReadable WRITE setReadable)
    Q_PROPERTY(bool writable READ isWritable WRITE setWritable)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(quint32 period READ period WRITE setPeriod)
    Q_PROPERTY(QString measureUnit READ measureUnit WRITE setMeasureUnit)

    Q_PROPERTY(int precision READ precision WRITE setPrecision NOTIFY precisionChanged)

    Q_PROPERTY(DataType serverValueDataType READ serverValueDataType WRITE setServerValueDataType)
    Q_PROPERTY(StrategyType valueStrategyType READ valueStrategyType WRITE setValueStrategyType)

    Q_PROPERTY(QVariant value READ value WRITE writeValue NOTIFY valueChanged)
    Q_PROPERTY(QVariant readValue READ readValue WRITE setValue)

    Q_PROPERTY(QVariant minValue READ minValue WRITE setMinValue)
    Q_PROPERTY(QVariant maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(QVariant defaultValue READ defaultValue WRITE setDefaultValue)

    Q_PROPERTY(QString permissibleVariation READ permissibleVariation WRITE setPermissibleVariation)

public:
    enum DataType {
        Boolean = 0,

        Int8 = 10,
        UInt8 = 11,
        Int16 = 12,
        UInt16 = 13,
        Int32 = 14,
        UInt32 = 15,
        Int64 = 16,
        UInt64 = 17,

        Float32 = 20,
        Float64 = 21,

        DateTime = 101,
        String = 201
    };
    Q_ENUM(DataType)

    enum StrategyType {
        NoneStrategy = 0,
        NumberStrategy,
        RegisterStrategy,
        EnumStrategy,
        BitsetStrategy
    };
    Q_ENUM(StrategyType)

    struct WriteData {
        QVariant writedServerValue;
        QVariant writedDeviceValue;
        QByteArray writedValueBytes;

        WriteData() {}
        WriteData(const QVariant &serverValue, const QVariant &deviceValue, const QByteArray &bytes) {
            writedServerValue = serverValue;
            writedDeviceValue = deviceValue;
            writedValueBytes = bytes;
        }

        bool isEmpty() {
            return  writedServerValue.isNull() || !writedServerValue.isValid() ||
                    writedDeviceValue.isNull() || !writedDeviceValue.isValid() ||
                    writedValueBytes.isEmpty();
        }

        inline bool isValid() {
            return !isEmpty();
        }
    };

    typedef SUPtr<WriteData> UniqueWriteDataPtr;
    typedef SSPtr<WriteData> SharedWriteDataPtr;

    explicit IVariable(QObject *parent = Q_NULLPTR);
    explicit IVariable(QDomElement *node, QObject *parent = Q_NULLPTR);
    virtual ~IVariable();

    // родительский узел
    INode *parentNode() const;
    void setParentNode(INode *parentNode);

    // имя переменной
    inline QString name() const;
    inline void setName(const QString &name);

    inline QString alias() const;
    inline void setAlias(const QString &alias);

    QString path() const;

    // описание переменной
    inline QString description() const;
    inline void setDescription(const QString &description);

    // разрешение на опрос
    bool isReadable() const;
    void setReadable(bool readable);

    bool isWritable() const;
    void setWritable(bool writable);

    inline bool isEnabled() const noexcept { return _enabled; }
    inline void setEnabled(bool enabled) noexcept { _enabled = enabled; }

    // период опроса
    quint32 period() const;
    void setPeriod(quint32 period);

    bool isBoolean() const;
    // размер в байтах
    quint32 dataSize() const;

    QDomElement *getConfigNode() const;
    void setConfigNode(QDomElement *element);

    inline QString measureUnit() const;
    inline void setMeasureUnit(const QString &measureUnit);

    // значение, хранимое в формате машины
    virtual QVariant value() const;
    virtual QVariant readValue(); /**< синхронное чтение */
    QVariant descriptionValue() const;

    QVariant valueFromDeviceVariantValue(const QVariant &deviceValue) const;

    UniqueWriteDataPtr writeValueDataForValue(const QVariant &value);

    inline DataType serverValueDataType() const;
    void setServerValueDataType(const DataType &serverValueDataType);
    bool serverValueDataTypeIsSigned() const;

    inline StrategyType valueStrategyType() const;
    void setValueStrategyType(const StrategyType &valueStrategyType);

    inline VariableValueStrategy *valueStrategy() const;

    virtual QVariant defaultValue() const;
    inline void setDefaultValue(const QVariant &defaultValue);

    inline QVariant minValue() const;
    inline void setMinValue(const QVariant &minValue);

    inline QVariant maxValue() const;
    inline void setMaxValue(const QVariant &maxValue);

    inline bool useMinMax() const;
    void setUseMinMax(bool useMinMax);

    inline QString permissibleVariation() const;
    void setPermissibleVariation(const QString &permissibleVariation);

    inline int precision() const;
    void setPrecision(int precision);

    bool hasError() const;
    NodeError lastError() const;

    void forceUpdateValue();

public slots:
    virtual void writeValue(const QVariant &value); /**< асинхронная запись */

    //TODO: возвращаемое значение должно быть типа bool
    virtual void setValue(const QVariant &value); /**< синхронная запись */

signals:
    void valueChanged(IVariable *);

    void writeDeviceValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes);
    void writeDeviceValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes);
    void readValueSync(IVariable *variable);

    void error() const;
    void precisionChanged();

protected:
    virtual IDeviceVariableValue *deviceVariableValue() const = 0;

    void setDeviceVariantValue(const QVariant &value);
    void setValueBytes(const QByteArray &bytes);
    virtual QByteArray convertByteOrder(const QByteArray &bytes) const = 0;

    void recreateValueStrategy();

    QByteArray writeValueBytes(const QVariant &value);

    virtual QVariant descriptionValue(const QVariant &value) const;

    void clearValue();
    void setLastError(const NodeError &error);

private:
    friend class INode;
    friend class IDeviceVariableValue;

    QVariant _minValue;
    QVariant _maxValue;
    QVariant _defaultValue;
    bool _useMinMax = true;

    IServerVariableValue *createServerVariableValue() const;
    VariableValueStrategy *createValueStrategy() const;
    void deviceValueChanged();

    INode *_parentNode = nullptr;
    QString _name;
    QString _alias;
    QString _path;
    QString _description;
    bool _readable = false;
    bool _writable = false;
    bool _enabled = true;
    quint32 _period = 0;
    QString _measureUnit;

    int _precision = 4;

    DataType _serverValueDataType;
    IServerVariableValue *_serverVariableValue = nullptr;

    StrategyType _valueStrategyType;
    VariableValueStrategy *_valueStrategy = nullptr;

    QDomElement *_configNode = nullptr;

    NodeError m_lastError = NodeError::noError();

    QString _permissibleVariation;
};

inline INode *IVariable::parentNode() const { return _parentNode; }
inline  void IVariable::setParentNode(INode *parentNode) { _parentNode = parentNode; }

inline QString IVariable::name() const { return _name; }
inline void IVariable::setName(const QString &name) { _name = name; }

inline QString IVariable::alias() const { return _alias; }
inline void IVariable::setAlias(const QString &alias) { _alias = alias; }

inline QString IVariable::description() const { return _description; }
inline void IVariable::setDescription(const QString &description) { _description = description; }

inline QString IVariable::measureUnit() const { return _measureUnit; }
inline void IVariable::setMeasureUnit(const QString &measureUnit) { _measureUnit = measureUnit; }

inline IVariable::DataType IVariable::serverValueDataType() const { return _serverValueDataType; }

inline IVariable::StrategyType IVariable::valueStrategyType() const { return _valueStrategyType; }

inline VariableValueStrategy *IVariable::valueStrategy() const { return _valueStrategy; }

inline void IVariable::setDefaultValue(const QVariant &defaultValue) { _defaultValue = defaultValue; }

inline QVariant IVariable::minValue() const { return _minValue; }
inline void IVariable::setMinValue(const QVariant &minValue) { _minValue = minValue; }

inline QVariant IVariable::maxValue() const { return _maxValue; }
inline void IVariable::setMaxValue(const QVariant &maxValue) { _maxValue = maxValue; }

inline bool IVariable::useMinMax() const { return _useMinMax; }

inline int IVariable::precision() const { return _precision; }

inline QString IVariable::permissibleVariation() const { return _permissibleVariation; }

#endif // IVARIABLE_H
