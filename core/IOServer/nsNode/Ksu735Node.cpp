#include "Ksu735Node.h"
#include "DBConfig/DBConfig.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

#include "Ksu735Crc.h"

#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/BitsetVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/EnumVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/NumberVariableValueStrategy.h"

#include "Logger/Logger2/LoggerTaskManager2.h"
#include "ReportManager/ParamsReport.h"

static const QString SapKsu735ParametsTag = "SAPKSU_735_PARAMETRS";
static const QString SapKsu0x20ParametrTag = "KSU0x20_PARAMETR";

Ksu735Node::Ksu735Node(int ksuSoftwareVersion, QObject *parent) :
    Ksu735Node(ksuSoftwareVersion, 0, parent)
{

}

Ksu735Node::Ksu735Node(int ksuSoftwareVersion, int ksuAddress, QObject *parent) :
    INode(parent),
    m_address(ksuAddress),
    m_ksuSoftwareVersion(ksuSoftwareVersion)
{
    loadSoftwareVersion();
    setName("KsuNode");
}

Ksu735Node::~Ksu735Node()
{

}

void Ksu735Node::readAllVariablesAsync()
{
    read0x10Variables(OperationTimingType::Async);
    read0x20Variables(m_ksu0x20Variables, OperationTimingType::Async);
}

bool Ksu735Node::readVariable(IVariable *variable, INode::OperationTimingType timingType)
{
    QList<IVariable *> variablesList;
    variablesList.append(variable);
    return readVariables(variablesList, timingType);
}

bool Ksu735Node::readVariables(const QList<IVariable *> &variables, OperationTimingType timingType)
{
    if (variables.isEmpty())
        return false;

    bool result = true;

    bool read0x10 = false;
    QList<Ksu735Variable *> read0x20VariablesList;

    for (auto variable : variables) {
        Ksu735Variable *ksuVariable = qobject_cast<Ksu735Variable *>(variable);
        if (!ksuVariable) continue;

        if (variable == m_statusWordVariable || variable == m_statusWordVariable)
            read0x10 = true;

        if (!read0x10 && m_ksu0x10VariablesHash.contains(ksuVariable->address(), ksuVariable))
            read0x10 = true;

        if (m_ksu0x20VariablesHash.value(ksuVariable->address()) == ksuVariable)
            read0x20VariablesList.append(ksuVariable);
    }

    if (!read0x10 && read0x20VariablesList.isEmpty()) {
        return false;
    }

    if (read0x10) {
        result &= read0x10Variables(timingType);
    }

    if (!read0x20VariablesList.isEmpty()) {
        result &= read0x20Variables(read0x20VariablesList, timingType);
    }

    return result;
}

bool Ksu735Node::writeVariable(IVariable *variable, const QVariant &writedValue, INode::OperationTimingType timingType)
{
    QPair<IVariable *, QVariant> writedValueData;
    writedValueData.first = variable;
    writedValueData.second = writedValue;

    QList<QPair<IVariable *, QVariant>> writedValuesDataList;
    writedValuesDataList.append(writedValueData);

    return writeVariables(writedValuesDataList, timingType);
}

bool Ksu735Node::writeVariables(const QList<QPair<IVariable *, QVariant>> &writedValuesData, INode::OperationTimingType timingType)
{
    if (writedValuesData.isEmpty())
        return false;

    bool result = true;

    QList<QList<QPair<Ksu735Variable *, QVariant>>> writeDataListOfList;

    int currentCount = 0;
    QList<QPair<Ksu735Variable *, QVariant>> tenWriteData;

    for (auto &data : writedValuesData) {
        Ksu735Variable *ksuVariable = qobject_cast<Ksu735Variable *>(data.first);

        if (!ksuVariable || data.second.isNull() || !data.second.isValid()) continue;
        if (m_ksu0x20VariablesHash.value(ksuVariable->address()) != ksuVariable) continue;

        currentCount++;
        tenWriteData.append(qMakePair(ksuVariable, data.second));

        if (currentCount == 10) {
            writeDataListOfList.append(tenWriteData);
            tenWriteData.clear();
            currentCount = 0;
        }
    }

    if (currentCount != 0) {
        writeDataListOfList.append(tenWriteData);
    }

    for (auto &list : writeDataListOfList) {
        result &= write0x20Variables(list, timingType);
    }

    return result;
}

bool Ksu735Node::readKsu0x10(INode::OperationTimingType timingType)
{
    return read0x10Variables(timingType);
}

Ksu735NodeArchive *Ksu735Node::readArchive(quint8 offset, bool *ok)
{
    QByteArray request = readKsu0x30ArchiveBytes(1, offset);
    SharedDataTaskPtr task = std::make_shared<ReadDataTask>(request, timeout());

    QByteArray response = emit handleTaskSync(request, timeout());

    QList<Ksu735NodeArchive *> archiveRecordsList;
    bool result = parseResponse(task, response);

    if (result) {
        archiveRecordsList = parse0x30Response(request, response);
    }

    if (ok) {
        *ok = result;
    }

    if (archiveRecordsList.size() != 1 || !ok) {
        return nullptr;
    }

    return archiveRecordsList.at(0);
}

QList<Ksu735NodeArchive *> Ksu735Node::readArchive(quint8 count, quint8 offset, bool *ok)
{
    QByteArray request = readKsu0x30ArchiveBytes(count, offset);
    SharedDataTaskPtr task = std::make_shared<ReadDataTask>(request, timeout());

    QByteArray response = emit handleTaskSync(request, timeout());

    QList<Ksu735NodeArchive *> archiveRecordsList;
    bool result = parseResponse(task, response);

    if (result) {
        archiveRecordsList = parse0x30Response(request, response);
    }

    if (ok) {
        *ok = result;
    }

    return archiveRecordsList;
}

QList<SharedPeriodicDataTaskPtr> Ksu735Node::makePeriodicReadTasks()
{
    return QList<SharedPeriodicDataTaskPtr>();

    QList<SharedPeriodicDataTaskPtr> tasks;

    auto read0x10task = std::make_shared<PeriodicReadDataTask>(readKsu0x10ParametersBytes());
    read0x10task->setPeriod(period());
    read0x10task->setTimeout(timeout());
    connect(read0x10task.get(), &IDataTask::receivedResponse, this, &Ksu735Node::parseResponse);

    tasks.append(read0x10task);


    QByteArrayList read0x20ParametrsBytes = readKsu0x20ParametersBytes();
    for (auto bytes : read0x20ParametrsBytes) {
        auto read0x20task = std::make_shared<PeriodicReadDataTask>(bytes);
        read0x20task->setPeriod(period());
        read0x20task->setTimeout(timeout());
        connect(read0x20task.get(), &IDataTask::receivedResponse, this, &Ksu735Node::parseResponse);

        tasks.append(read0x20task);
    }

    return tasks;
}

bool Ksu735Node::parseResponse(SharedDataTaskPtr task, const QByteArray &rx)
{
    const QByteArray &tx = task->request();

    qDebug() << "Ksu0x10Parametrs735Node parseResponse";
    qDebug() << "TX:" << tx.toHex().toUpper();
    qDebug() << "RX:" << rx.toHex().toUpper();

    if (rx.isEmpty()) {
        /** устройство не отвечает */
        setLastError(NodeError::noResponseError());
        errorReport(task, rx);
        return false;
    }

    if (!Ksu735Crc::getInstance()->exactMatch(rx)) {
        /** ошибка crc */
        setLastError(NodeError::crcError());
        errorReport(task, rx);
        return false;
    }

    if (rx[2] != tx[2]) {
        int errorCode = rx[2] & 0x0F;

        /** ошибка с ксу */
        if (m_protocolErrors.contains(errorCode)) {
            setLastError(NodeError::nativeDeviceError(errorCode, m_protocolErrors[errorCode]));
        } else {
            /** неизвестная ошибка(ошибки нет в списке ошибок) */
            setLastError(NodeError::unknownDeviceError());
        }

        errorReport(task, rx);
        return false;
    }

    if (rx.length() < 6 ||
            quint8(rx.at(3)) != rx.length() - 6 || /**< 6 байт - это шапка и crc */
            (tx[2] == 0x10 && rx.length() < 12) ||
            (tx[2] == 0x20 && quint8(tx.at(3)) * 3 != rx.at(3)) ||
            (tx[2] == 0x30 && quint8(tx.at(5)) * 8 != rx.length() - 7)) {
        //неверный формат пакета - слишком мало данных
        setLastError(NodeError::packageFormatError());
        errorReport(task, rx);
        return false;
    }

    if (tx[1] != rx[1]) {
        /** неизвестный адрес */
        setLastError(NodeError::packageFormatError());
        errorReport(task, rx);
        return false;
    }

    if (tx[2] == 0x10) {
        parse0x10Response(tx, rx);
        ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Read, tx, rx, QString(), QString(), QString::number(true));
    } else if (tx[2] == 0x20) {
        parse0x20Response(tx, rx);
    }
    setLastError(NodeError::noError());

    if (tx[2] == 0x40) {
        successReport(task, rx);
    } else {
        if (task->userData())
            delete task->userData();
    }

    return true;
}

void Ksu735Node::writeValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes)
{
    Ksu735Variable *ksuVariable = qobject_cast<Ksu735Variable *>(variable);
    if (!ksuVariable ||
            m_ksu0x20VariablesHash.value(ksuVariable->address()) != ksuVariable ||
            bytes.isEmpty() ||
            value.isNull() ||
            !value.isValid())
        return;

    QList<QPair<Ksu735Variable *, QByteArray>> requestData;
    requestData.append(qMakePair(ksuVariable, bytes));

    QByteArray request = writeKsu0x20VariableBytes(requestData);

    auto reportData = new Ksu735Node::VariablesData();
    reportData->type = Ksu735Node::VariablesData::Write;
    Ksu735Node::VariablesData::VariableOperationData operationData;
    operationData.variable = ksuVariable->address();
    operationData.value = value;
    reportData->data.append(operationData);

    SharedDataTaskPtr task = std::make_shared<WriteDataTask>(variable, value, request, timeout(), reportData);

    emit handleTaskAsync(task);
}

void Ksu735Node::writeValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes)
{
    Ksu735Variable *ksuVariable = qobject_cast<Ksu735Variable *>(variable);
    if (!ksuVariable ||
            m_ksu0x20VariablesHash.value(ksuVariable->address()) != ksuVariable ||
            bytes.isEmpty() ||
            value.isNull() ||
            !value.isValid())
        return;

    QList<QPair<Ksu735Variable *, QByteArray>> requestData;
    requestData.append(qMakePair(ksuVariable, bytes));

    auto reportData = new Ksu735Node::VariablesData();
    reportData->type = Ksu735Node::VariablesData::Write;
    Ksu735Node::VariablesData::VariableOperationData operationData;
    operationData.variable = ksuVariable->address();
    operationData.value = value;
    reportData->data.append(operationData);

    QByteArray request = writeKsu0x20VariableBytes(requestData);
    SharedDataTaskPtr task = std::make_shared<WriteDataTask>(variable, value, request, timeout(), reportData);

    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);
    QByteArray response = emit handleTaskSync(request, timeout());

    if (parseResponse(task, response))
        LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
    else
        LoggerTaskManager2::getInstance()->log("Устройство не отвечает", LoggerTaskManager2::Error);
    // TODO: запись в архив п. 5.7.1
    // request = tx
    // response = rx
}

void Ksu735Node::readValueSync(IVariable *variable)
{
    QList<IVariable *> list;
    list.append(variable);

    readVariables(list, OperationTimingType::Sync);
}

bool Ksu735Node::read0x10Variables(OperationTimingType timingType)
{
    QByteArray request = readKsu0x10ParametersBytes();
    SharedDataTaskPtr task = std::make_shared<ReadDataTask>(request, timeout());

    if (timingType == OperationTimingType::Sync) {
        LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);
        QByteArray response = emit handleTaskSync(request, timeout());

        // TODO: запись в архив п. 5.7.1
        // tx = tx
        // rx = rx
        bool result = parseResponse(task, response);
        if (result)
            LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
        else
            LoggerTaskManager2::getInstance()->log("Устройство не отвечает", LoggerTaskManager2::Error);
        return result;
    } else {
        emit handleTaskAsync(task);
    }
    return true;
}

bool Ksu735Node::read0x20Variables(const QList<Ksu735Variable *> variables, INode::OperationTimingType timingType)
{
    bool result = true;

    auto reportData = new Ksu735Node::VariablesData();
    reportData->type = Ksu735Node::VariablesData::Write;
    Ksu735Node::VariablesData::VariableOperationData operationData;
    for (auto variable : variables) {
        if (variable) {
            operationData.variable = variable->address();
            reportData->data.append(operationData);
        }
    }

    QByteArrayList requestsList = readKsu0x20ParametersBytes(variables);
    for (auto request : requestsList) {
        SharedDataTaskPtr task = std::make_shared<ReadDataTask>(request, timeout(), reportData);
        LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);
        if (timingType == OperationTimingType::Sync) {
            QByteArray response = emit handleTaskSync(request, timeout());
            result &= parseResponse(task, response);

            if (result)
                LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
            else
                LoggerTaskManager2::getInstance()->log("Устройство не отвечает", LoggerTaskManager2::Error);
        } else {
            emit handleTaskAsync(task);
        }
    }

    // TODO: запись в архив п. 5.7.1
    // tx = tx
    // rx = rx

    return result;
}

bool Ksu735Node::write0x20Variables(const QList<QPair<Ksu735Variable *, QVariant>> &writedValuesData,
                                    INode::OperationTimingType timingType)
{
    QMap<IVariable *, QVariant> writedDeviceValues;
    QList<QPair<Ksu735Variable *, QByteArray>> requestData;

    auto reportData = new Ksu735Node::VariablesData();
    reportData->type = Ksu735Node::VariablesData::Write;
    Ksu735Node::VariablesData::VariableOperationData operationData;

    for (auto writedValue : writedValuesData) {
        IVariable::UniqueWriteDataPtr writeDataPtr = writedValue.first->writeValueDataForValue(writedValue.second);
        IVariable::WriteData *writeData = writeDataPtr.get();

        if (!writeData || !writeData->isValid()) continue;

        writedDeviceValues.insert(writedValue.first, writeData->writedDeviceValue);
        requestData.append(qMakePair(writedValue.first, writeData->writedValueBytes));

        operationData.variable = writedValue.first->address();
        operationData.value = writedValue.second;
        reportData->data.append(operationData);
    }

    if (writedDeviceValues.isEmpty() || requestData.isEmpty())
        return false;

    QByteArray request = writeKsu0x20VariableBytes(requestData);
    SharedDataTaskPtr task = std::make_shared<WriteDataTask>(writedDeviceValues, request, timeout(), reportData);
    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);
    if (timingType == OperationTimingType::Sync) {
        QByteArray response = emit handleTaskSync(request, timeout());

        // TODO: запись в архив п. 5.7.1 (имеет смысл только для синхронных запросов, т.к от асинхронных ответ может прийти когда угодно)
        // request = tx
        // response = rx
        bool result = parseResponse(task, response);
        if (result) {
            LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
        } else {
            QString logString = lastError().description() + " (RX:" + response.toHex() + ")";
            LoggerTaskManager2::getInstance()->log(logString, LoggerTaskManager2::Error);
        }
        return result;
    } else {
        emit handleTaskAsync(task);
    }

    return true;
}

QByteArray Ksu735Node::readKsu0x10ParametersBytes() const
{
    QByteArray request;
    request.append(char(0x00)); /**< Все посылки начинаются с нуля */
    request.append(char(QString::number(m_address, 10).toInt(0, 16))); /**< Адрес КСУ (BDС) */
    request.append(char(0x10)); /**< Функция */
    request.append(char(0x00)); /**< длина блока */
    request.append(Ksu735Crc::getInstance()->calcCrc(request, 0, request.size())); /**< crc */
    return request;
}

QByteArray Ksu735Node::readKsu0x20VariableBytes(Ksu735Variable *variable) const
{
    return readKsu0x20TenParametersBytes(QList<Ksu735Variable *>() << variable);
}

QByteArray Ksu735Node::readKsu0x20TenParametersBytes(const QList<Ksu735Variable *> &parameters) const
{
    int parametersCount = qMin(parameters.length(), 10);
    if (parametersCount == 0)
        return QByteArray();

    QByteArray variableAddresses;
    for (int i = 0; i < parametersCount; i++) {
        const Ksu735Variable *variable = parameters.at(i);
        variableAddresses.append(char(static_cast<quint8>(variable->address())));
    }

    QByteArray request;
    request.append(char(0x00)); /**< Все посылки начинаются с нуля */
    request.append(char(QString::number(m_address, 10).toInt(0, 16))); /**< Адрес КСУ (BDС) */
    request.append(char(0x20)); /**< Функция */
    request.append(char(parametersCount)); /**< длина блока */
    request.append(variableAddresses);
    request.append(Ksu735Crc::getInstance()->calcCrc(request, 0, request.size()));

    return request;
}

QByteArrayList Ksu735Node::readKsu0x20ParametersBytes() const
{
    return readKsu0x20ParametersBytes(m_ksu0x20Variables);
}

QByteArrayList Ksu735Node::readKsu0x20ParametersBytes(const QList<Ksu735Variable *> &parameters) const
{
    if (parameters.isEmpty())
        return QByteArrayList();

    QByteArrayList result;
    QList<Ksu735Variable *> readTenParameters;

    int currentCount = 0;
    for (auto variable : parameters) {
        currentCount++;
        readTenParameters.append(variable);

        if (currentCount == 10) {
            result.append(readKsu0x20TenParametersBytes(readTenParameters));
            readTenParameters.clear();
            currentCount = 0;
        }
    }

    if (currentCount != 0) {
        result.append(readKsu0x20TenParametersBytes(readTenParameters));
    }

    return result;
}

QByteArray Ksu735Node::readKsu0x30ArchiveBytes(quint8 count, quint8 offset) const
{
    QByteArray request;
    request.append(char(0x00)); /**< Все посылки начинаются с нуля */
    request.append(char(QString::number(m_address, 10).toInt(0, 16))); /**< Адрес КСУ (BDС) */
    request.append(char(0x30)); /**< Функция */
    request.append(char(0x02)); /**< длина блока */
    request.append(char(offset)); /**< смещение */
    request.append(char(count)); /**< количество записей */
    request.append(Ksu735Crc::getInstance()->calcCrc(request, 0, request.size()));

    return request;
}

QByteArray Ksu735Node::writeKsu0x20VariableBytes(const QList<QPair<Ksu735Variable *, QByteArray>> &writedValuesData) const
{
    QByteArray bytes;
    for (auto data : writedValuesData) {
        bytes.append(static_cast<quint8>(data.first->address()));
        bytes.append(data.second);
    }

    QByteArray request;
    request.append(char(0x00)); /**< Все посылки начинаются с нуля */
    request.append(char(QString::number(m_address, 10).toInt(0, 16))); /**< Адрес КСУ (BDС) */
    request.append(char(0x40)); /**< Функция */
    request.append(char(bytes.length() + 2)); /**< длина блока + 2 байта на резерв?? */
    request.append(bytes);
    request.append(QByteArray(2, 0)); /**< резерв */
    request.append(Ksu735Crc::getInstance()->calcCrc(request, 0, request.size()));

    return request;

    //    request
    //            .append(char(0x00))
    //            .append(char(QString::number(_address, 10).toUInt(0, 16))) //Адрес КСУ (BDС))
    //            .append(char(0x40))
    //            .append(3) //Аналог 16 функции modbus - запись нескольких регистров крайне маловероятна. Пока пишем по одному.
    //            .append(char(variable->address()))
    //            .append(bytes)
    //            .append(QByteArray::fromHex("00 00")) //"Резерв" (C)
    //            .append(Ksu735Crc::getInstance()->calcCrc(request, 0, request.size()));
}

QList<Ksu735Variable *> Ksu735Node::variablesByTx(const QByteArray &tx) const
{
    QList<Ksu735Variable *>();
}

void Ksu735Node::setVariablesError(const QList<Ksu735Variable *> &variables, const NodeError &error)
{

}

void Ksu735Node::parse0x10Response(const QByteArray &tx, const QByteArray &rx)
{
    qDebug() << "parse0x10Response" << tx.toHex().toUpper() << rx.toHex().toUpper();

    QSet<Ksu735Variable *> notReadedVariable = m_ksu0x10Variables.toSet();

    if (m_statusWordVariable) {
        m_statusWordVariable->setValueBytes(rx.mid(4, 2));

        notReadedVariable.remove(m_statusWordVariable);

        qDebug() << "Получено значение параметра по 10 функции - " << m_statusWordVariable->address() << m_statusWordVariable->name()
                 << " = " << m_statusWordVariable->value() << m_statusWordVariable->measureUnit();
    }

    if (m_stopReasonVariable) {
        QByteArray stopReasonBytes = QByteArray(1, rx.at(6));
        stopReasonBytes.append(char(0x00));
        m_stopReasonVariable->setValueBytes(stopReasonBytes);

        notReadedVariable.remove(m_stopReasonVariable);

        qDebug() << "Получено значение параметра по 10 функции - " << m_stopReasonVariable->address() << m_stopReasonVariable->name()
                 << " = " << m_stopReasonVariable->value() << m_stopReasonVariable->measureUnit();
    }

    QByteArray parametrsData = rx;
    parametrsData.chop(2);
    parametrsData.remove(0, 10);

    quint32 flags = quint8(rx[9]) << 16 | quint8(rx[8]) << 8 | quint8(rx[7]);

    int byteOffset = 0;
    for (int bitOffset = 23; bitOffset >= 0; bitOffset--) {
        if (!((flags >> bitOffset) & 1))
            continue;

        auto variables = m_ksu0x10VariablesHash.values(1 << bitOffset);
        for (auto variable : variables) {
            if (!variable) continue;
            variable->setValueBytes(parametrsData.mid(byteOffset, 2));

            notReadedVariable.remove(variable);

            qDebug() << "Получено значение параметра по 10 функции - " << variable->address() << variable->name()
                     << " = " << variable->value() << variable->measureUnit();
        }

        byteOffset += 2;
    }

    for (auto variable : notReadedVariable) {
        variable->clearValue();
    }
}

void Ksu735Node::parse0x20Response(const QByteArray &tx, const QByteArray &rx)
{
    Q_UNUSED(tx);

    quint8 parametrsCount = static_cast<quint8>(tx.at(3));

    QByteArray parametrsData = rx;
    parametrsData.chop(2);
    parametrsData.remove(0, 4);

    for (int i = 0; i < parametrsCount; i++) {
        int offset = i * 3;
        quint32 address = static_cast<quint8>(parametrsData.at(offset));
        auto variable = m_ksu0x20VariablesHash.value(address, nullptr);
        if (!variable) continue;

        QByteArray variableValueBytes = parametrsData.mid(offset + 1, 2);
        variable->setValueBytes(variableValueBytes);

        ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Read, tx, rx, variable->alias(), variable->value().toString(), QString::number(true));

        qDebug() << "Получено значение параметра по 20 функции - " << variable->address() << variable->name()
                 << " = " << variable->value() << variable->measureUnit();
    }
}

QList<Ksu735NodeArchive *> Ksu735Node::parse0x30Response(const QByteArray &tx, const QByteArray &rx)
{
    QList<Ksu735NodeArchive *> archiveRecordsList;

    QByteArray bytes = rx;
    bytes.chop(2);
    bytes.remove(0, 5);

    quint8 archiveRecordsCount = static_cast<quint8>(tx.at(5));
    for (int i = 0; i < archiveRecordsCount; i++) {
        auto archiveRecord = new Ksu735NodeArchive(this, bytes.mid(i * 8, 8), this);
        archiveRecordsList.append(archiveRecord);
    }

    return archiveRecordsList;
}

void Ksu735Node::loadSoftwareVersion()
{
    if (m_ksuSoftwareVersion == -1)
        return;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `KsuSoftwareVersions`"
                                "WHERE id = %1;")
                        .arg(m_ksuSoftwareVersion);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect() || !query.first())
        return;

    bool ok;
    int protocolType = query.value(QStringLiteral("protocolType")).toInt(&ok);
    if (protocolType || !ok)
        return;

    int protocolCode = query.value(QStringLiteral("comProtocolCode")).toInt(&ok);
    if (ok) {
        setupProtocolErrors(protocolCode);
        setupStatusWordVariable(protocolCode);
        setupStopReasonVariable(protocolCode);
        setupKsu0x10Variables(protocolCode);
    }

    QString parametrsConfigFileName = query.value(QStringLiteral("parametrsConfigPath")).toString();
    QDir parametrsConfigDir = QDir(DBConfig::ksuParametrsFolderPath());
    QString parametrsConfigPath = parametrsConfigDir.absoluteFilePath(parametrsConfigFileName);
    setupKsu0x20Variables(parametrsConfigPath);

    QList<IVariable *> variables;

    for (auto var : m_ksu0x10Variables) {
        variables.append(var);
    }
    for (auto var : m_ksu0x20Variables) {
        variables.append(var);
    }
    appendVariables(variables);
}

void Ksu735Node::setupProtocolErrors(int comProtocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT code, name FROM `KSU753ErrorCodes`"
                                "WHERE comProtocolCode = %1;")
                        .arg(comProtocolCode);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect())
        return;

    query.seek(-1);
    bool ok;
    while (query.next()) {
        int code = query.value(QStringLiteral("code")).toInt(&ok);
        if (!ok) continue;

        QString name = query.value(QStringLiteral("name")).toString();
        m_protocolErrors.insert(code, name);
    }
}

void Ksu735Node::setupStatusWordVariable(int comProtocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT code, name FROM `StatusWordInterpretationCodes`"
                                "WHERE comProtocolCode = %1 ORDER BY code DESC;")
                        .arg(comProtocolCode);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect())
        return;

    QHash<quint64, QString> bitsValues;

    query.seek(-1);
    bool ok;
    while (query.next()) {
        quint64 code = query.value(QStringLiteral("code")).toULongLong(&ok);
        if (!ok) continue;

        QString name = query.value(QStringLiteral("name")).toString();
        bitsValues.insert(code, name);
    }

    Ksu735Variable *statusWordVariable = new Ksu735Variable(this);
    statusWordVariable->setAddress(0);
    statusWordVariable->setName(QStringLiteral("Состояние скважины"));
    statusWordVariable->setValueStrategyType(IVariable::BitsetStrategy);
    statusWordVariable->setReadable(true);
    statusWordVariable->setAlias("KSU735x10_status_word");
    statusWordVariable->setValueConversionType(Ksu735DeviceVariableValue::ConversionType::Bin);

    BitsetVariableValueStrategy *strategy = dynamic_cast<BitsetVariableValueStrategy *>(statusWordVariable->valueStrategy());
    if (strategy) {
        strategy->setBitsValues(bitsValues);
    }

    m_statusWordVariable = statusWordVariable;
    m_statusWordVariable->setUseMinMax(false);
    m_ksu0x10Variables.append(m_statusWordVariable);
}

void Ksu735Node::setupStopReasonVariable(int comProtocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT code, name FROM `StopReasonCodes`"
                                "WHERE comProtocolCode = %1 ORDER BY code ASC;")
                        .arg(comProtocolCode);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect())
        return;

    QMap<quint64, QString> enumValues;

    query.seek(-1);
    bool ok;
    while (query.next()) {
        quint64 code = query.value(QStringLiteral("code")).toULongLong(&ok);
        if (!ok) continue;

        QString name = query.value(QStringLiteral("name")).toString();
        enumValues.insert(code, name);
        m_stopReasonCodes.insert(code, name);
    }

    Ksu735Variable *stopReasonVariable = new Ksu735Variable(this);
    stopReasonVariable->setAddress(0);
    stopReasonVariable->setName(QStringLiteral("Первопричина останова"));
    stopReasonVariable->setValueStrategyType(IVariable::EnumStrategy);
    stopReasonVariable->setReadable(true);
    stopReasonVariable->setAlias("KSU735x10_stop_reason");
    stopReasonVariable->setValueConversionType(Ksu735DeviceVariableValue::ConversionType::Bin);

    EnumVariableValueStrategy *strategy = dynamic_cast<EnumVariableValueStrategy *>(stopReasonVariable->valueStrategy());
    if (strategy) {
        strategy->setEnumValues(enumValues);
    }

    m_stopReasonVariable = stopReasonVariable;
    m_stopReasonVariable->setUseMinMax(false);
    m_ksu0x10Variables.append(m_stopReasonVariable);
}

void Ksu735Node::setupKsu0x10Variables(int comProtocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `Return0x10FunctionParametrsFlags`"
                                "WHERE comProtocolCode = %1 ORDER BY code DESC;")
                        .arg(comProtocolCode);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect())
        return;

    QMetaEnum metaEnum = QMetaEnum::fromType<SapKsu::Ksu735ParametrFormat>();
    query.seek(-1);
    bool ok;
    while (query.next()) {
        quint64 code = query.value(QStringLiteral("code")).toULongLong(&ok);
        if (!ok) continue;

        int intFormat = query.value(QStringLiteral("format")).toInt(&ok);
        if (!ok) continue;

        int index = metaEnum.value(intFormat);
        if (index == -1) continue;

        SapKsu::Ksu735ParametrFormat format = static_cast<SapKsu::Ksu735ParametrFormat>(index);

        Ksu735Variable *variable = new Ksu735Variable(this);
        variable->setAddress(code);
        variable->setName(query.value(QStringLiteral("parametrName")).toString());
        variable->setAlias(query.value(QStringLiteral("alias")).toString());
        variable->setReadable(true);
        variable->setFormat(format);
        variable->setUseMinMax(false);

        m_ksu0x10Variables.append(variable);
        m_ksu0x10VariablesHash.insert(code, variable);

        if (format == SapKsu::XX1XX) {
            QList<Ksu735Variable *> specialVariables = special0x10VariablesFromVariable(variable, comProtocolCode);
            m_ksu0x10Variables.append(specialVariables);

            for (auto var : specialVariables) {
                var->setUseMinMax(false);
                m_ksu0x10VariablesHash.insert(code, var);
            }
        }
    }
}

void Ksu735Node::setupKsu0x20Variables(const QString &parametrsPath)
{
    QFile configFile(parametrsPath);
    if (!configFile.exists())
        return;

    if (!configFile.open(QIODevice::ReadOnly)) {
        qCritical() << tr("Не удается открыть файл конфигурации") << configFile.errorString();
        return;
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    QDomDocument xmlConfig;
    if (!xmlConfig.setContent(&configFile, &errorMsg, &errorLine, &errorColumn)) {
        qCritical() << tr("Конфигурационный файл поврежден") << errorMsg << errorLine << errorColumn;
        return;
    }
    traverse0x20ConfigXml(xmlConfig);

    //    QMetaEnum formatMetaEnum = QMetaEnum::fromType<SapKsu::Ksu735ParametrFormat>();
    //    for (auto var : m_ksu0x20Variables)
    //        qDebug() << "var" << var->address() << var->name() << formatMetaEnum.valueToKey(var->format());
}

QList<Ksu735Variable *> Ksu735Node::special0x10VariablesFromVariable(const Ksu735Variable *variable,
                                                                     int comProtocolCode) const
{
    QList<Ksu735Variable *> variables;

    QStringList variableNames = variable->name().split('+', QString::SkipEmptyParts);
    if (variableNames.length() != 2)
        return variables;

    QString workModeVariableName = variableNames.at(0).trimmed();
    Ksu735Variable *workModeVariable = createWorkModeVariable(variable, workModeVariableName, comProtocolCode);
    if (workModeVariable)
        variables.append(workModeVariable);

    QString kindOfLaunchVariableName = variableNames.at(1).trimmed();
    Ksu735Variable *kindOfLaunchVariable = createKindOfLaunchVariable(variable, kindOfLaunchVariableName, comProtocolCode);
    if (kindOfLaunchVariable)
        variables.append(kindOfLaunchVariable);

    return variables;
}

Ksu735Variable *Ksu735Node::createWorkModeVariable(const Ksu735Variable *variable,
                                                   const QString &name,
                                                   int comProtocolCode) const
{
    if (name.isEmpty() || !variable)
        return nullptr;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `WorkMode`"
                                "WHERE comProtocolCode = %1 ORDER BY code ASC;")
                        .arg(comProtocolCode);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect())
        return nullptr;

    QMap<quint64, QString> enumValues;

    query.seek(-1);
    bool ok;
    while (query.next()) {
        quint64 code = query.value(QStringLiteral("code")).toULongLong(&ok);
        if (!ok) continue;

        QString name = query.value(QStringLiteral("name")).toString();
        enumValues.insert(code, name);
    }

    Ksu735Variable *workModeVariable = new Ksu735Variable(variable->parent());
    workModeVariable->setAddress(variable->address());
    workModeVariable->setName(name);
    workModeVariable->setReadable(variable->isReadable());
    workModeVariable->setFormat(SapKsu::XXXX);
    workModeVariable->setValueConversionType(Ksu735DeviceVariableValue::ConversionType::Bin);
    workModeVariable->setValueStrategyType(Ksu735Variable::EnumStrategy);

    EnumVariableValueStrategy *strategy = dynamic_cast<EnumVariableValueStrategy *>(workModeVariable->valueStrategy());
    if (strategy) {
        strategy->setEnumValues(enumValues);
        strategy->setStartBit(8);
        strategy->setEndBit(15);
    }

    return workModeVariable;
}

Ksu735Variable *Ksu735Node::createKindOfLaunchVariable(const Ksu735Variable *variable,
                                                       const QString &name,
                                                       int comProtocolCode) const
{
    if (name.isEmpty() || !variable)
        return nullptr;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `KindOfLaunch`"
                                "WHERE comProtocolCode = %1 ORDER BY code ASC;")
                        .arg(comProtocolCode);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect())
        return nullptr;

    QMap<quint64, QString> enumValues;

    query.seek(-1);
    bool ok;
    while (query.next()) {
        quint64 code = query.value(QStringLiteral("code")).toULongLong(&ok);
        if (!ok) continue;

        QString name = query.value(QStringLiteral("name")).toString();
        enumValues.insert(code, name);
    }

    Ksu735Variable *kindOfLaunchVariable = new Ksu735Variable(variable->parent());
    kindOfLaunchVariable->setAddress(variable->address());
    kindOfLaunchVariable->setName(name);
    kindOfLaunchVariable->setReadable(variable->isReadable());
    kindOfLaunchVariable->setFormat(SapKsu::XXXX);
    kindOfLaunchVariable->setValueConversionType(Ksu735DeviceVariableValue::ConversionType::Bin);
    kindOfLaunchVariable->setValueStrategyType(Ksu735Variable::EnumStrategy);

    EnumVariableValueStrategy *strategy = dynamic_cast<EnumVariableValueStrategy *>(kindOfLaunchVariable->valueStrategy());
    if (strategy) {
        strategy->setEnumValues(enumValues);
        strategy->setStartBit(0);
        strategy->setEndBit(7);
    }

    return kindOfLaunchVariable;
}

void Ksu735Node::traverse0x20ConfigXml(const QDomNode &domNode)
{
    QDomNode node = domNode.firstChild();

    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement element = node.toElement();
            if (element.tagName() == SapKsu735ParametsTag) {
                QDomNodeList parametrsList = element.childNodes();
                int count = parametrsList.count();
                for (int i = 0; i < count; i++) {
                    QDomNode parametrNode = parametrsList.at(i);
                    if (parametrNode.isNull() || !parametrNode.isElement()) continue;

                    QDomElement parametrElement = parametrNode.toElement();
                    if (parametrElement.tagName() != SapKsu0x20ParametrTag) continue;

                    Ksu735Variable *variable = new Ksu735Variable(this);
                    variable->setReadable(true);

                    QDomNamedNodeMap attributesMap = parametrElement.attributes();
                    QStringList parametrAttributes = QStringList() << "address"         << "name"
                                                                   << "alias"           << "writable"
                                                                   << "enabled"         << "format"
                                                                   << "measureUnit"     << "permissibleVariation";

                    for (QString &attributeName : parametrAttributes) {
                        if (attributesMap.contains(attributeName)) {
                            QString attributeNodeName = attributesMap.namedItem(attributeName).nodeName();
                            variable->setProperty(attributeNodeName.toLocal8Bit().constData(),
                                                  parametrElement.attribute(attributeNodeName));
                        }
                    }

                    if (variable->name().isEmpty()) {
                        variable->setName(tr("Зарезервирован"));
                    }

                    SapKsu::Ksu735ParametrFormat variableFormat = variable->format();

                    if (variableFormat == SapKsu::Ksu735ParametrFormat::BIN && attributesMap.contains(QStringLiteral("measureUnit"))) {
                        QString measureUnitStr = parametrElement.attribute(QStringLiteral("measureUnit"));
                        if (measureUnitStr.toLower().startsWith(QStringLiteral("bitset:"))) {
                            variable->setMeasureUnit(QString());

                            measureUnitStr = measureUnitStr.remove(QStringLiteral("bitset:")).simplified();

                            variable->setValueStrategyType(IVariable::BitsetStrategy);

                            if (BitsetVariableValueStrategy *strategy =
                                dynamic_cast<BitsetVariableValueStrategy *>(variable->valueStrategy())) {
                                QHash<quint64, QString> bitsValues;
                                QStringList mapStringList = measureUnitStr.split(";/", QString::SkipEmptyParts);
                                for (const QString &oneValue : mapStringList) {
                                    QStringList mapElementStringList = oneValue.split("->");
                                    if (mapElementStringList.length() == 2 && !mapElementStringList.at(1).isEmpty()) {
                                        bool ok = false;
                                        QString keyString = mapElementStringList.at(0);
                                        quint64 key = keyString.toULongLong(&ok, 0);
                                        if (!ok) continue;
                                        bitsValues.insert(key, mapElementStringList.at(1));
                                    }
                                }
                                strategy->setBitsValues(bitsValues);
                            }
                        }
                    }


                    if (attributesMap.contains("minValue")) {
                        variable->setMinValue(correctedParametrValueFromFormat(variableFormat,
                                                                               parametrElement.attribute(
                                                                                   "minValue")));
                        if (variableFormat == SapKsu::Ksu735ParametrFormat::CCCC) {
                            variable->setMinValue(0);
                        }
                    }

                    if (attributesMap.contains("maxValue")) {
                        variable->setMaxValue(correctedParametrValueFromFormat(variableFormat,
                                                                               parametrElement.attribute(
                                                                                   "maxValue")));
                        if (variableFormat == SapKsu::Ksu735ParametrFormat::CCCC) {
                            variable->setMaxValue(65535);
                        }
                    }

                    if (attributesMap.contains("defaultValue")) {
                        variable->setDefaultValue(correctedParametrValueFromFormat(variableFormat,
                                                                                   parametrElement.attribute(
                                                                                       "defaultValue")));
                    }

                    //FIXME: времянка - надо править конфиги для 7.35 формата CCCC
                    if (variableFormat == SapKsu::Ksu735ParametrFormat::CCCC) {
                        variable->setMinValue(0);
                        variable->setMaxValue(65535);
                        variable->setDefaultValue(0);
                    }

                    variable->setUseMinMax(false);
                    m_ksu0x20Variables.append(variable);
                    m_ksu0x20VariablesHash.insert(variable->address(), variable);
                }
            }
        }

        traverse0x20ConfigXml(node);
        node = node.nextSibling();
    }
}

QVariant Ksu735Node::correctedParametrValueFromFormat(SapKsu::Ksu735ParametrFormat format,
                                                      const QString &value) const noexcept
{
    if (format == SapKsu::Ksu735ParametrFormat::CCCC)
        return QVariant();

    if (value.trimmed().isEmpty())
        return QVariant();

    bool ok;
    quint32 intValue = value.toUInt(&ok, 0);

    if (!ok) return QVariant();

    QVariant result = QVariant();

    int divisor = SapKsu::paramertValueDivisorForFormat(format);
    if (divisor == 1) {
        result.setValue(intValue);
    } else {
        double fixedValue = double(intValue) / divisor;
        result.setValue(fixedValue);
    }

    return result;
}

void Ksu735Node::successReport(SharedDataTaskPtr task, const QByteArray &rx)
{
    report(task->request(), rx, true, task->userData());
}

void Ksu735Node::errorReport(SharedDataTaskPtr task, const QByteArray &rx)
{
    report(task->request(), rx, false, task->userData());
}

void Ksu735Node::report(const QByteArray &tx, const QByteArray &rx, bool result, void *data)
{
    if (tx[2] == 0x30)
        return;

    if (!data) {
        ParamsReport::ParamsReportFunction function = ParamsReport::ParamsReportFunction::Read;
        if (tx[2] == 0x40) {
            function = ParamsReport::ParamsReportFunction::Write;
        }
        ParamsReport::instance()->write(function, tx, rx, QString(), QString(), QString::number(result));
        return;
    }

    Ksu735Node::VariablesData *variablesData = static_cast<Ksu735Node::VariablesData *>(data);
    writeReport(tx, rx, result, *variablesData);

    delete variablesData;
}

void Ksu735Node::writeReport(const QByteArray &tx, const QByteArray &rx, bool result, const Ksu735Node::VariablesData &variablesData)
{
    ParamsReport::ParamsReportFunction function = ParamsReport::ParamsReportFunction::Read;
    if (variablesData.type == VariablesData::Write) {
        function = ParamsReport::ParamsReportFunction::Write;
    }

    if (variablesData.data.isEmpty()) {
        ParamsReport::instance()->write(function, tx, rx, QString(), QString(), QString::number(result));
    }
    for (const Ksu735Node::VariablesData::VariableOperationData &data : variablesData.data) {
        QString alias;
        Ksu735Variable *variable = variableByAddress(data.variable);
        if (variable) {
            alias = variable->alias();
        }

        ParamsReport::instance()->write(function, tx, rx, alias, data.value.toString(), QString::number(result));
    }
}

//    00 01 10 00 1100 tx
//    00 01 10 20 2401 01 0f80ff 0000 0000 0000 0000 1300 0000 0000 0000 0000 0101 0000 0000 0000 fa01 rx raw
//    00 01 10 20
//                  2401 01
//                           0f80ff
//                                    0000 0000 0000 0000 1300 0000 0000 0000 0000 0101 0000 0000 0000
//                                                                                                      fa01 rx

// ff800f
//        0000 0000 0000 0000 1300 0000 0000 0000 0000 0101 0000 0000 0000

//    Получено значение параметра по 10 функции -  "Активная мощность"  =  QVariant(ushort, 0) "кВт"
//    Получено значение параметра по 10 функции -  "Ток потребления Ia"  =  QVariant(ushort, 0) "А"
//    Получено значение параметра по 10 функции -  "Ток потребления Ib"  =  QVariant(ushort, 0) "кВт"
//    Получено значение параметра по 10 функции -  "Ток потребления Ic"  =  QVariant(ushort, 0) "кВт"
//    Получено значение параметра по 10 функции -  "Сопротивление изоляции"  =  QVariant(ushort, 13) "кОм"
//    Получено значение параметра по 10 функции -  "К-т мощности (cosF)"  =  QVariant(ushort, 0) ""
//    Получено значение параметра по 10 функции -  "Напряжение Ua"  =  QVariant(ushort, 0) "В"
//    Получено значение параметра по 10 функции -  "Напряжение Ub"  =  QVariant(ushort, 0) "В"
//    Получено значение параметра по 10 функции -  "Напряжение Uc"  =  QVariant(ushort, 0) "В"
//    Получено значение параметра по 10 функции -  "Режим работы \\ Вид последнего запуска"  =  QVariant(ushort, 101) ""
//    Получено значение параметра по 10 функции -  "Загрузка ПЭД"  =  QVariant(ushort, 0) "%"
//    Получено значение параметра по 10 функции -  "Дисбаланс токов"  =  QVariant(ushort, 0) "%"


//    Получено значение параметра по 10 функции -  "Активная мощность"  =  QVariant(ushort, 0) "кВт"
//    Получено значение параметра по 10 функции -  "Ток потребления Ia"  =  QVariant(ushort, 0) "А"
//    Получено значение параметра по 10 функции -  "Ток потребления Ib"  =  QVariant(ushort, 0) "кВт"
//    Получено значение параметра по 10 функции -  "Ток потребления Ic"  =  QVariant(ushort, 0) "кВт"
//    Получено значение параметра по 10 функции -  "Сопротивление изоляции"  =  QVariant(ushort, 13) "кОм"
//    Получено значение параметра по 10 функции -  "К-т мощности (cosF)"  =  QVariant(ushort, 0) ""
//    Получено значение параметра по 10 функции -  "Напряжение Ua"  =  QVariant(ushort, 0) "В"
//    Получено значение параметра по 10 функции -  "Напряжение Ub"  =  QVariant(ushort, 0) "В"
//    Получено значение параметра по 10 функции -  "Напряжение Uc"  =  QVariant(ushort, 0) "В"
//    Получено значение параметра по 10 функции -  "Режим работы \\ Вид последнего запуска"  =  QVariant(ushort, 101) ""
//    Получено значение параметра по 10 функции -  "Загрузка ПЭД"  =  QVariant(ushort, 0) "%"
//    Получено значение параметра по 10 функции -  "Дисбаланс токов"  =  QVariant(ushort, 0) "%"

//      0001200A000102030405060708095800
//      0001200a000102030405060708095800

//TX:  "0001200a0a0b0c0d0e0f10111213bc00"
//RX:  "0001201e0a00000b00000c00000d00000e00000f00001054651100001200001300008901"
//Получено значение параметра по 20 функции -  "Темп. жидкости на выкиде"  =  QVariant(ushort, 0) "С"
//Получено значение параметра по 20 функции -  "Темп. жидкости на приеме"  =  QVariant(ushort, 0) "С"
//Получено значение параметра по 20 функции -  "Темп. обмотки ПЭД"  =  QVariant(ushort, 0) "C"
//Получено значение параметра по 20 функции -  "Вибрация УЭЦН X + Y коорд"  =  QVariant(ushort, 0) "g"
//Получено значение параметра по 20 функции -  "Вибрация УЭЦН Y коорд (В протоколе нет)"  =  QVariant(ushort, 0) "g"
//Получено значение параметра по 20 функции -  "Вибрация УЭЦН Z коорд"  =  QVariant(ushort, 0) "g"
//Получено значение параметра по 20 функции -  "Давление на выкиде насоса"  =  QVariant(ushort, 6554) "мПа"
//Получено значение параметра по 20 функции -  "Давление в буфере (В протоколе нет)"  =  QVariant(ushort, 0) "кВт"
//Получено значение параметра по 20 функции -  "Давление в линии (В протоколе нет)"  =  QVariant(ushort, 0) "кВт"
//Получено значение параметра по 20 функции -  "Расход жидкости на выкиде"  =  QVariant(ushort, 0) "м3\\сут"

//    Ksu0x10Parametrs735Node parseResponse ?????
//    TX: "0001200A000102030405060708095800"
//    RX: "0001201E0000000100000200000300000413000500000600000700000800000954653801"


//    Ksu0x10Parametrs735Node parseResponse ?????
//    TX: "0001200A000102030405060708095800"
//    RX: "0001201E0000000100000200000300000413000500000600000700000800000954653801"
//    QSerialPort::SerialPortError(TimeoutError) "COM8"
//    Ksu0x10Parametrs735Node parseResponse
//    TX: "0001200A0A0B0C0D0E0F10111213BC00"
//    RX: "0001201E0A00000B00000C00000D00000E00000F000010546511000012000013000089010001201E0A00000B00000C00000D00000E00000F00001054651100001200001300008901"
//    QSerialPort::SerialPortError(TimeoutError) "COM8"
//    Ksu0x10Parametrs735Node parseResponse
//    TX: "0001200A1415161718191A1B1C1D2001"
//    RX: "0001201E1401011500001600001700001800001900001A00001B00001C00001D00003601"

//    00 01 20 0A 14 15 16 17 18 19 1A 1B 1C 1D 2001
//    00 01 20 1E
//                14 0101
//                15 0000
//                16 0000
//                17 0000
//                18 0000
//                19 0000
//                1A 0000
//                1B 0000
//                1C 0000
//                1D 0000
//                            3601


// 0x30
// TX: 00 01 30 02 01 06 3A 00

// RX: 000130310103031412FF280000030313120402270003031312161427020303121202040403030312120606520203030912FF0100006E04
// RX: 00 01 30 31 01
//                      03 03 14 12 FF 28 0000
//                      03 03 13 12 04 02 2700
//                      0303131216142702
//                      0303121202040403
//                      0303121206065202
//                      03030912FF010000
//                                          6E04

//     00 01 30 02 0A 06 4300
//     00 01 30 31 0A
//                      0722291102030000
//                      0722271104020000
//                      07222611FF090000
//                      12030308FF090000
//                      0000000000000000
//                      0000000000000000
//                                          CB03

// TODO: проверить парсинг 0x20
//TX: "00012001FA1C01"
//RX: "000123002400"
