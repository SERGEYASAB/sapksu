#include "ModbusCrc.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsTask/IDataTask.h"
#include "IOServer/nsNode/INode.h"
#include "IOServer/nsNode/ModbusNode.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/nsVariable/ModbusVariable.h"

#include "Logger/LoggerTaskManager.h"
#include "Logger/Logger2/LoggerTaskManager2.h"

#include <QDebug>
#include <QDateTime>

#include <iostream>
#include <memory>

ModbusNode::ModbusNode(QObject *parent) :
    INode(parent)
{

}

ModbusNode::ModbusNode(QDomElement &node, QObject *parent)
    : INode(node, parent)
{
    m_protocolErrors.insert(ModbusError::IllegalFunction, QStringLiteral("Недопустимый код запроса"));
    m_protocolErrors.insert(ModbusError::IllegalDataAddress, QStringLiteral("Недопустимый адрес"));
    m_protocolErrors.insert(ModbusError::IllegalDataValue, QStringLiteral("Недопустимое значение параметра"));
    m_protocolErrors.insert(ModbusError::DeviceFailure, QStringLiteral("Невосстанавливаемая ошибка имела место, пока ведомое устройство пыталось выполнить затребованное действие"));
    m_protocolErrors.insert(ModbusError::DeviceProcessing, QStringLiteral("Ведомое устройство приняло запрос и обрабатывает его, но это требует много времени"));
    m_protocolErrors.insert(ModbusError::DeviceIsBusy, QStringLiteral("Ведомое устройство занято обработкой команды"));
    m_protocolErrors.insert(ModbusError::FunctionCannotPerformed, QStringLiteral("Ведомое устройство не может выполнить программную функцию, заданную в запросе"));
    m_protocolErrors.insert(ModbusError::DeviceMemoryParityError, QStringLiteral(" Ведомое устройство при чтении расширенной памяти обнаружило ошибку контроля четности"));
}

bool ModbusNode::parseResponse(SharedDataTaskPtr task, const QByteArray &rx)
{
    bool result = false;
    const QByteArray &tx = task->request();
    //    qDebug() << "parseResponse" << tx.toHex().toUpper() << rx.toHex().toUpper();

    if (rx.isEmpty()) {
        setLastError(NodeError::noResponseError());
        return false;
    }

    // Для получения полной информации о пакете раскоментируй строку ниже:
    //    qDebug() << QDateTime::currentDateTime() << tx.toHex().toUpper() << rx.toHex().toUpper();

    if (validateResponse(tx, rx)) {
        const QByteArray txPdu = getModbusPdu(tx);
        const QByteArray rxPdu = getModbusPdu(rx);

        //        qDebug() << "parse pdu" << QDateTime::currentDateTime() << txPdu.toHex().toUpper() << rxPdu.toHex().toUpper();

        //        qDebug() << QDateTime::currentDateTime() << txPdu.toHex().toUpper() << rxPdu.toHex().toUpper();
        //        LoggerTaskManager::getInstance()->log(txPdu.toHex().toUpper() +
        //                                              rxPdu.toHex().toUpper(),
        //                                              LoggerTaskManager::Info);


        quint8 rxFunction = rxPdu[ModbusRequestField::Function];
        bool modbusError = (rxFunction >> 7) & 0x01;

        if (!modbusError) {
            quint32 startAddress = (quint32(quint8(txPdu[ModbusRequestField::StartAddressHi])) << 8) |
                    quint8(txPdu[ModbusRequestField::StartAddressLo]);
            quint32 registersCount = (quint32(quint8(txPdu[ModbusRequestField::QuantityHi])) << 8) |
                    quint8(txPdu[ModbusRequestField::QuantityLo]);

            // 0x01 && 0x02
            if (txPdu[ModbusRequestField::Function] == ModbusReadFunction::ReadCoils ||
                    txPdu[ModbusRequestField::Function] == ModbusReadFunction::ReadDiscreteInputs) {

                quint16 bytesCount = rxPdu[2];
                //                qDebug() << "bytesCount" << bytesCount;

                QByteArray rxData = rxPdu;
                rxData.remove(0, 3);

                //                qDebug() << "rxData" << rxData.toHex().toUpper();
                quint64 flagsData = 0;
                Q_ASSERT(rxData.size() == bytesCount);
                for (int i = 0; i < rxData.size(); i++) {
                    flagsData |= quint64((quint8)rxData[i]) << i * 8;
                }

                for (auto variable : variables()) {
                    qDebug()<<variable->alias();
                    ModbusVariable *var = static_cast<ModbusVariable *>(variable);

                    if ((var->address() >= startAddress) && (var->address() + 1 <= startAddress + registersCount)) {
                        quint32 offset = var->address() - startAddress;
                        bool result = flagsData & (1 << offset);

                        var->setDeviceVariantValue(QVariant(result));
                    }
                }
            }

            // 0x03 && 0x04
            if (txPdu[ModbusRequestField::Function] == ModbusReadFunction::ReadHoldingRegisters ||
                    txPdu[ModbusRequestField::Function] == ModbusReadFunction::ReadInputRegister) {

                QByteArray registersData(rxPdu);
                registersData.remove(0, 3);

                for (auto variable : variables()) {

                    ModbusVariable *var = static_cast<ModbusVariable *>(variable);

                    if ((var->address() >= startAddress) &&
                            (var->address() + var->registersCount() <= startAddress + registersCount)) {
                        quint32 offset = (var->address() - startAddress) * 2;

                        QByteArray varUpdateData(registersData);
                        varUpdateData.truncate(offset + var->dataSize());
                        varUpdateData.remove(0, offset);

                        var->setValueBytes(varUpdateData);
                    }
                }
            }

            if (txPdu[ModbusRequestField::Function] == ModbusWriteFunction::WriteSingleCoil
                    || txPdu[ModbusRequestField::Function] == ModbusWriteFunction::WriteSingleRegister
                    || txPdu[ModbusRequestField::Function] == ModbusWriteFunction::WriteMultipleCoils
                    || txPdu[ModbusRequestField::Function] == ModbusWriteFunction::WriteMultipleRegisters) {

                //qDebug() << "parse pdu" << QDateTime::currentDateTime() << txPdu.toHex().toUpper() << rxPdu.toHex().toUpper();


                std::shared_ptr<WriteDataTask> writeTask = std::dynamic_pointer_cast<WriteDataTask>(task);
                if (!writeTask) {
                    //                    qDebug() << "Для записи modbus переменной был использован не WriteDataTask, установка значения невозможна";

                }
                return true;

                //                if (!writeTask->variable()) {
                //                    qDebug() << "В объекте записи нет указателя на переменную";
                //                }

                //                ModbusVariable *var = static_cast<ModbusVariable *>(writeTask->variable());
                //                var->setDeviceVariantValue(writeTask->writedValue());
            }
        } else {
            ModbusError errorCode = static_cast<ModbusError>(rxPdu[2]);
            //            qDebug() << "Ошибка на устройстве" << errorCode << rxPdu.toHex().toUpper();
            setLastError(NodeError::nativeDeviceError(errorCode, m_protocolErrors[errorCode]));
            return false;
        }
    } else {
        /// "Ошибка валидации данных";

        setLastError(NodeError::packageFormatError());
        return false;
    }

    setLastError(NodeError::noError());
    return true;
}

void ModbusNode::writeValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes)
{
    ModbusVariable *modbusVariable = qobject_cast<ModbusVariable *>(variable);
    if (!modbusVariable || !modbusVariable->isWritable())
        return;

    QByteArray request = dataForWriteValueTask(modbusVariable, value, bytes);
    if (request.isEmpty())
        return;

    QByteArray requestAdu = getModbusAdu(request);
    SharedDataTaskPtr task = std::make_shared<WriteDataTask>(modbusVariable, value, requestAdu, timeout());

    connect(task.get(), &IDataTask::receivedResponse, this, &ModbusNode::parseResponse);
    emit handleTaskAsync(task);
}

void ModbusNode::writeValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes)
{
    ModbusVariable *modbusVariable = qobject_cast<ModbusVariable *>(variable);
    if (!modbusVariable || !modbusVariable->isWritable()) {
        return;
    }

    QByteArray request = dataForWriteValueTask(modbusVariable, value, bytes);
    if (request.isEmpty())
        return;

    QByteArray requestAdu = getModbusAdu(request);
    SharedDataTaskPtr task = std::make_shared<WriteDataTask>(modbusVariable, value, requestAdu, timeout());
    task->setTimeout(timeout());

    //    qDebug() << "writeDataTask sync" << request.toHex().toUpper() << requestAdu.toHex().toUpper() << value;

#ifdef ST_BP_M5
    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);
#endif

    QByteArray response = emit handleTaskSync(requestAdu, timeout());


    //    qDebug() << "writeDataTask sync response" << requestAdu.toHex().toUpper() << response.toHex().toUpper();
    //    qDebug() << "writeDataTask sync variable old value" << variable->value();

#ifdef ST_BP_M5
    if (parseResponse(task, response)) {
        LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
    } else {
        LoggerTaskManager2::getInstance()->log("Устройство не отвечает", LoggerTaskManager2::Error);
    }
#else
    parseResponse(task, response);
#endif


    if (m_writeCallback)
        (*m_writeCallback)(variable, value, request, response);



    //    qDebug() << "writeDataTask sync variable new value" << variable->value();
}

void ModbusNode::readValueSync(IVariable *variable)
{
    ModbusVariable *modbusVariable = qobject_cast<ModbusVariable *>(variable);
    if (!modbusVariable || !modbusVariable->isReadable()) {
        return;
    }

    QByteArray request;
    request[Address] = _address;
    request[Function] = _readFunction;

    request[StartAddressHi] = modbusVariable->address() >> 8;
    request[StartAddressLo] = modbusVariable->address();

    request[QuantityHi] = modbusVariable->registersCount() >> 8;
    request[QuantityLo] = modbusVariable->registersCount();

    QByteArray requestAdu = getModbusAdu(request);
    SharedDataTaskPtr task = std::make_shared<ReadDataTask>(requestAdu);

#ifdef ST_BP_M5
    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);
#endif

    QByteArray response = emit handleTaskSync(requestAdu, timeout());

    //    qDebug() << "readDataTask sync" << request.toHex().toUpper() << requestAdu.toHex().toUpper();
    //    qDebug() << "readDataTask sync variable old value" << variable->value();

#ifdef ST_BP_M5
    if (parseResponse(task, response)) {
        LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
    } else {
        LoggerTaskManager2::getInstance()->log("Устройство не отвечает", LoggerTaskManager2::Error);
    }
#else
    parseResponse(task, response);
#endif

    if (m_readCallback)
        (*m_readCallback)(variable, request, response);

    //    qDebug() << "readDataTask sync variable new value" << variable->value();
}

QByteArray ModbusNode::dataForWriteValueTask(ModbusVariable *variable, const QVariant &value, const QByteArray &bytes) const
{
    QByteArray request;

    //TODO: заменить qCritical записью в лог
    if (_writeFunction == ModbusWriteFunction::WriteSingleCoil) {
        if (!value.canConvert<bool>()) {
            //            qCritical() << "Попытка записи по 5 функции невалидного значения" << variable->value() << "переменной"
            //                        << variable->name() << "в ноде" << name();
            return QByteArray();
        }

        request[ModbusRequestField::Address] = _address;
        request[ModbusRequestField::Function] = _writeFunction;

        request[ModbusRequestField::StartAddressHi] = variable->address() >> 8;
        request[ModbusRequestField::StartAddressLo] = variable->address();

        if (value.toBool()) {
            request[0x04] = 0xFF;
        } else {
            request[0x04] = 0x00;
        }
        request[0x05] = 0x00;
    } else if (_writeFunction == ModbusWriteFunction::WriteSingleRegister) {
        if (variable->registersCount() > 1) {
            //            qCritical() << "Попытка использовать 6 функцию для записи переменной" << variable->name() << "в ноде" << name()
            //                        << "\r\nПроверьте конфиг";
            return QByteArray();
        }

        request[ModbusRequestField::Address] = _address;
        request[ModbusRequestField::Function] = _writeFunction;

        request[ModbusRequestField::StartAddressHi] = variable->address() >> 8;
        request[ModbusRequestField::StartAddressLo] = variable->address();

        request.append(bytes);
    } else if (_writeFunction == ModbusWriteFunction::WriteMultipleCoils) {
        if (!value.canConvert<bool>()) {
            //            qCritical() << "Попытка записи по 5 функции невалидного значения" << variable->value() << "переменной"
            //                        << variable->name() << "в ноде" << name();
            return QByteArray();
        }

        request[ModbusRequestField::Address] = _address;
        request[ModbusRequestField::Function] = _writeFunction;

        request[ModbusRequestField::StartAddressHi] = variable->address() >> 8;
        request[ModbusRequestField::StartAddressLo] = variable->address();

        request[ModbusRequestField::QuantityHi] = 0x00;
        request[ModbusRequestField::QuantityLo] = 0x01;

        request[ModbusRequestField::BytesCount] = 1;
        request[0x07] = value.toBool();

        //        qDebug() << "WriteMultipleCoils request" << request.toHex().toUpper();
    } else if (_writeFunction == ModbusWriteFunction::WriteMultipleRegisters) {
        request[ModbusRequestField::Address] = _address;
        request[ModbusRequestField::Function] = _writeFunction;

        request[ModbusRequestField::StartAddressHi] = variable->address() >> 8;
        request[ModbusRequestField::StartAddressLo] = variable->address();

        request[ModbusRequestField::QuantityHi] = variable->registersCount() >> 8;
        request[ModbusRequestField::QuantityLo] = variable->registersCount();

        request[ModbusRequestField::BytesCount] = bytes.count();

        request.append(bytes);
    }

    return request;
}

quint8 ModbusNode::hollowRegistersCount() const noexcept
{
    return _hollowRegistersCount;
}

void ModbusNode::setHollowRegistersCount(quint8 hollowRegistersCount) noexcept
{
    _hollowRegistersCount = hollowRegistersCount;
}

quint8 ModbusNode::address() const noexcept
{
    return _address;
}

void ModbusNode::setAddress(quint8 address) noexcept
{
    _address = address;
}

ModbusNode::ModbusReadFunction ModbusNode::readFunction() const
{
    return _readFunction;
}

void ModbusNode::setReadFunction(const ModbusReadFunction &readFunction)
{
    _readFunction = readFunction;
}

ModbusNode::ModbusWriteFunction ModbusNode::writeFunction() const
{
    return _writeFunction;
}

void ModbusNode::setWriteFunction(const ModbusWriteFunction &writeFunction)
{
    _writeFunction = writeFunction;
}

bool ModbusNode::groupRequest() const noexcept
{
    return _groupRequest;
}

void ModbusNode::setGroupRequest(bool groupRequest) noexcept
{
    _groupRequest = groupRequest;
}

quint8 ModbusNode::maxLengthPdu() const noexcept
{
    return _maxLengthPdu;
}

void ModbusNode::setMaxLengthPdu(quint8 maxLengthPdu) noexcept
{
    _maxLengthPdu = maxLengthPdu;
}

QString ModbusNode::relayAddresses() const noexcept
{
    return _relayAddresses;
}

void ModbusNode::setRelayAddresses(const QString &relayAddresses) noexcept
{
    _relayAddresses = relayAddresses;
}

QString ModbusNode::firstCommingAddresses() const noexcept
{
    return _firstCommingAddresses;
}

void ModbusNode::setFirstCommingAddresses(const QString &firstCommingAddresses) noexcept
{
    _firstCommingAddresses = firstCommingAddresses;
}

void ModbusNode::appendVariable(IVariable *variable)
{
    ModbusVariable *modbusVariable = qobject_cast<ModbusVariable *>(variable);
    if (!modbusVariable) {
        return;
    }

    if ((_writeFunction == ModbusWriteFunction::WriteSingleCoil || _writeFunction == ModbusWriteFunction::WriteSingleRegister) &&
            modbusVariable->isWritable() && modbusVariable->registersCount() > 1) {
        //        qInfo() << "Попытка использовать функцию записи одного регистра с переменной большего типа"
        //                << modbusVariable->name() << "в ноде" << name()
        //                << "\r\nПроверьте конфиг";
        return;
    }

    INode::appendVariable(modbusVariable);
}

// QMap<период, QMap<адрес, ModbusVariable *>>

// Заполняет контейнер (из 2х переменных с одинаковым адресом выберется большая по dataSize)
// за счет QMap сортирует переменные по адресам:

// Период 1: Переменные:     Период 2: Переменные:     Период 3: Переменные:
//           Адрес 0                   Адрес 100                 Адрес 500
//           Адрес 2                   Адрес 150                 Адрес 501
//           Адрес 4                   Адрес 160                 Адрес 502
//           Адрес 6                   Адрес 161
//           Адрес 8                   Адрес 200

QMap<quint32, QMap<quint16, ModbusVariable *>> ModbusNode::sortModbusMap() const
{
    QList<IVariable *> readVariables = getReadVariables();
    QMap<quint32, QMap<quint16, ModbusVariable *>> sortModbusMap;

    for (auto variable : readVariables) {
        ModbusVariable *modbusVariable = static_cast<ModbusVariable *>(variable);
        QMap<quint16, ModbusVariable *> &map = sortModbusMap[variable->period()];

        if (!map.contains(modbusVariable->address())) {
            map.insert(modbusVariable->address(), modbusVariable);
        } else {
            const ModbusVariable *oldVariable = map[modbusVariable->address()];
            if (modbusVariable->dataSize() > oldVariable->dataSize()) {
                map.insert(modbusVariable->address(), modbusVariable);
            }
        }
    }

    return sortModbusMap;
}

void ModbusNode::setWriteValueCallback(WriteValueCallback callback)
{
    m_writeCallback = callback;
}

void ModbusNode::setReadValueCallback(ReadValueCallback callback)
{
    m_readCallback = callback;
}

QByteArray ModbusNode::readRegistersRequestBytes(quint8 function, quint16 registerAddress, quint16 registersCount) const
{
    QByteArray request;
    request[Address] = _address;
    request[Function] = function;

    request[StartAddressHi] = registerAddress >> 8;
    request[StartAddressLo] = registerAddress;

    request[QuantityHi] = registersCount >> 8;
    request[QuantityLo] = registersCount;

    QByteArray requestAdu = getModbusAdu(request);

    return requestAdu;
}

QByteArray ModbusNode::writeSingleRegisterBytes(quint8 function, quint16 registerAddress, quint16 value) const
{
    QByteArray valueBytes;
    valueBytes[0] = value >> 8;
    valueBytes[1] = value;

    return writeSingleRegisterBytes(function, registerAddress, valueBytes);
}

QByteArray ModbusNode::writeSingleRegisterBytes(quint8 function, quint16 registerAddress, const QByteArray &bytes) const
{
    QByteArray request;
    request[Address] = _address;
    request[Function] = function;

    request[StartAddressHi] = registerAddress >> 8;
    request[StartAddressLo] = registerAddress;

    request.append(bytes);

    QByteArray requestAdu = getModbusAdu(request);

    return requestAdu;
}

QByteArray ModbusNode::writeMultipleRegistersBytes(quint8 function, quint16 registerAddress, quint16 value) const
{
    QByteArray valueBytes;
    valueBytes[0] = value >> 8;
    valueBytes[1] = value;

    return writeMultipleRegistersBytes(function, registerAddress, 1, valueBytes);
}

QByteArray ModbusNode::writeMultipleRegistersBytes(quint8 function, quint16 registerAddress, quint16 registersCount, const QByteArray &bytes) const
{
    QByteArray request;
    request[Address] = _address;
    request[Function] = function;

    request[StartAddressHi] = registerAddress >> 8;
    request[StartAddressLo] = registerAddress;

    request[QuantityHi] = registersCount >> 8;
    request[QuantityLo] = registersCount;

    request[ModbusRequestField::BytesCount] = bytes.count();

    request.append(bytes);

    QByteArray requestAdu = getModbusAdu(request);

    return requestAdu;
}

QByteArray ModbusNode::readVariableBytes(const ModbusVariable *variable) const
{

#ifdef SAP_KSU
#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS

    QStringList aliases04List {"KSU_WORKMODE_PED",
                               "KSU_STATE_PED",
                               "KSU_STOPPING_REASON",
                               "KSU_U_IN_AB",
                               "KSU_U_IN_BC",
                               "KSU_U_IN_CA",
                               "KSU_Ia",
                               "KSU_Ib",
                               "KSU_Ic",
                               "KSU_I_disbalance",
                               "KSU_U_disbalance",
                               "KSU_loading_ped",
                               "KSU_POWER_FULL",
                               "KSU_POWER_ACTIVE",
                               "KSU_R",
                               "KSU_frequency_turbine_rotation",
                               "KSU_U_OUT_FC",
                               "KSU_P_pumpinput",
                               "KSU_DATE_YEAR",
                               "KSU_DATE_MONTH",
                               "KSU_DATE_DAY",
                               "KSU_DATE_HOUR",
                               "KSU_DATE_MINUTE",
                               "KSU_DATE_SECOND"
                              };

    for (auto alias : aliases04List) {
        if (variable != nullptr) {
            if (variable->alias() == alias) {
                return readVariableBytes(ModbusNode::ReadInputRegister, variable);
            }
        }
    }

    if (variable != nullptr) {
        if (variable->alias() == "KSU_start_stop") {
            return readVariableBytes(ModbusNode::ReadCoils, variable);
        }
    }

#endif

    return readVariableBytes(readFunction(), variable);
}

QByteArray ModbusNode::readVariableBytes(quint8 function, const ModbusVariable *variable) const
{
    QByteArray request;
    request[Address] = _address;
    request[Function] = function;

    request[StartAddressHi] = variable->address() >> 8;
    request[StartAddressLo] = variable->address();

    request[QuantityHi] = variable->registersCount() >> 8;
    request[QuantityLo] = variable->registersCount();

    QByteArray requestAdu = getModbusAdu(request);

    return requestAdu;
}

QByteArray ModbusNode::writeVariableBytes(const ModbusVariable *variable, const QVariant &value, const QByteArray &bytes) const
{

#ifdef SAP_KSU
#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS

    if (variable != nullptr) {
        if (variable->alias() == "KSU_start_stop") {
            ModbusVariable *variable2 = const_cast<ModbusVariable *>(variable);
            variable2->readValue();
            return writeVariableBytes(ModbusNode::WriteSingleCoil, variable, value, bytes);
        }
    }

#endif

    return writeVariableBytes(writeFunction(), variable, value, bytes);
}

QByteArray ModbusNode::writeVariableBytes(quint8 function, const ModbusVariable *variable, const QVariant &value, const QByteArray &bytes) const
{
    QByteArray request;

    request[ModbusRequestField::Address] = _address;
    request[ModbusRequestField::Function] = function;

    request[ModbusRequestField::StartAddressHi] = variable->address() >> 8;
    request[ModbusRequestField::StartAddressLo] = variable->address();

    if (function == ModbusWriteFunction::WriteSingleCoil) {
        if (value.toBool()) {
            request[0x04] = 0xFF;
        } else {
            request[0x04] = 0x00;
        }
        request[0x05] = 0x00;
    } else if (function == ModbusWriteFunction::WriteSingleRegister && bytes.size() == 2 && variable->registersCount() == 1) {
        request.append(bytes);
    } else if (function == ModbusWriteFunction::WriteMultipleCoils) {
        request[ModbusRequestField::QuantityHi] = 0x00;
        request[ModbusRequestField::QuantityLo] = 0x01;

        request[ModbusRequestField::BytesCount] = 1;
        request[0x07] = value.toBool();
    } else { // ModbusWriteFunction::WriteMultipleRegisters
        request[ModbusRequestField::QuantityHi] = variable->registersCount() >> 8;
        request[ModbusRequestField::QuantityLo] = variable->registersCount();

        request[ModbusRequestField::BytesCount] = bytes.count();

        request.append(bytes);
    }

    QByteArray requestAdu = getModbusAdu(request);

    return requestAdu;
}

QList<ModbusVariable *> ModbusNode::variablesByTx(const QByteArray &tx) const
{
    return QList<ModbusVariable *>();
}

void ModbusNode::setVariablesError(const QList<ModbusVariable *> &variables, const NodeError &error)
{

}

const QHash<int, QString> &ModbusNode::protocolErrors() const
{
    return m_protocolErrors;
}

QList<SharedPeriodicDataTaskPtr> ModbusNode::makePeriodicReadTasks()
{
    QStringList firstCommingAddressesStrings = _firstCommingAddresses.split(" ");
    QVector<uint> firstCommingAddresses;
    for (QString string : firstCommingAddressesStrings) {
        bool ok = false;
        uint address = string.toUInt(&ok);
        if (ok) {
            firstCommingAddresses.push_back(address);
        }
    }

    QList<SharedPeriodicDataTaskPtr> tasks;

    QMap<quint32, QMap<quint16, ModbusVariable *>> sortedModbusMap = sortModbusMap();

    struct TaskData {
        quint32 period;
        quint16 startAddress;
        quint16 registersCount;
    };
    QVector<TaskData> taskDataList;

    QMap<quint32, QMap<quint16, ModbusVariable *>>::const_iterator periodsMapIterator = sortedModbusMap.cbegin();
    while (periodsMapIterator != sortedModbusMap.cend()) {
        quint32 period = periodsMapIterator.key();
        QMap<quint16, ModbusVariable *> variables = periodsMapIterator.value();

        auto variablesMapIterator = variables.cbegin();
        if (!_groupRequest) {
            while (variablesMapIterator != variables.cend()) {
                ModbusVariable *variable = variablesMapIterator.value();
                TaskData taskData = {period, variable->address(), variable->registersCount()};
                taskDataList.push_back(taskData);
                variablesMapIterator++;
            }
        } else {
            TaskData *taskData = Q_NULLPTR;
            while (variablesMapIterator != variables.cend()) {
                ModbusVariable *variable = variablesMapIterator.value();

                if (taskData) {
                    bool createGap = false;

                    // 1 условие обрыва: по значениям адресов из конфига
                    for (uint address : firstCommingAddresses) {
                        if (taskData->startAddress < address && variable->address() >= address) {
                            createGap = true;
                            break;
                        }
                    }

                    // 2 условие обрыва: по пропускам регистров
                    if (!createGap &&
                            taskData->startAddress + taskData->registersCount + _hollowRegistersCount < variable->address()) {
                        createGap = true;
                    }

                    // 3 условие обрыва: по максимально допустимому размеру ответа с девайса
                    qint16 addedRegisters = variable->address() + variable->registersCount() -
                            taskData->startAddress - taskData->registersCount;
                    if (!createGap && addedRegisters > 0 && (taskData->registersCount + addedRegisters) * 2 > _maxLengthPdu) {
                        createGap = true;
                    }

                    if (createGap) {
                        taskDataList.push_back(*taskData);
                        delete taskData;
                        taskData = Q_NULLPTR;
                    }
                }

                if (!taskData) {
                    taskData = new TaskData{period, variable->address(), 0};
                }

                qint16 addedRegisters = variable->address() + variable->registersCount() - taskData->startAddress - taskData->registersCount;
                if (addedRegisters > 0) {
                    taskData->registersCount += addedRegisters;
                }
                variablesMapIterator++;
            }

            if (taskData) {
                taskDataList.append(*taskData);
                delete taskData;
                taskData = Q_NULLPTR;
            }
        }

        periodsMapIterator++;
    }


    for (TaskData taskData : taskDataList) {
        QByteArray request;
        request[Address] = _address;
        request[Function] = _readFunction;

        request[StartAddressHi] = taskData.startAddress >> 8;
        request[StartAddressLo] = taskData.startAddress;

        request[QuantityHi] = taskData.registersCount >> 8;
        request[QuantityLo] = taskData.registersCount;

        auto periodicReadDataTask = std::make_shared<PeriodicReadDataTask>(getModbusAdu(request));
        periodicReadDataTask->setPeriod(taskData.period);
        periodicReadDataTask->setTimeout(timeout());

        connect(periodicReadDataTask.get(), &IDataTask::receivedResponse,
                this, &ModbusNode::parseResponse);


        tasks.append(periodicReadDataTask);
    }

    return tasks;
}
