#include "ModbusKsuNode.h"

#include "DBConfig/DBConfig.h"
#include "IOServer/nsVariable/ModbusVariable.h"

#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/BitsetVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/EnumVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/NumberVariableValueStrategy.h"

#include "ModbusKsuArchive/ModbusKsuCurrentParametrsArchive.h"
#include "ModbusKsuArchive/ModbusKsuChangeParametrsArchive.h"
#include "ModbusKsuArchive/ModbusKsuEventsArchive.h"
#include "ModbusKsuArchive/ModbusKsuLaunchParametrsArchive.h"

#include "Logger/Logger2/LoggerTaskManager2.h"
#include "ReportManager/ParamsReport.h"

static const QString SapksuModbusParametrsMapNodeTag = "SAPKSU_MODBUS_PARAMETRS_MAP";
static const QString VariableNodeTagName        = "IVariable";
static const QString VariableNodeElementId      = "ModbusVariable";

ModbusKsuNode::ModbusKsuNode(int ksuSoftwareVersion, int ksuAddress, QObject *parent) :
    ModbusRtuNode(parent),
    m_ksuSoftwareVersion(ksuSoftwareVersion)
{
    setAddress(ksuAddress);
    loadSoftwareVersion();

#ifdef SAP_KSU
    setReadFunction(ModbusNode::ReadHoldingRegisters);
    setWriteFunction(ModbusNode::WriteMultipleRegisters);
#endif
#ifdef SAP_SU
    setReadFunction(ModbusNode::ReadHoldingRegisters);
    setWriteFunction(ModbusNode::WriteMultipleRegisters);
#endif
#ifdef MECHOS
    setReadFunction(ModbusNode::ReadHoldingRegisters);
    setWriteFunction(ModbusNode::WriteMultipleRegisters);
#endif

    setName("KsuNode");
    setWriteValueCallback(writeCallback);
    setReadValueCallback(readCallback);
}

ModbusKsuNode::~ModbusKsuNode()
{

}

bool ModbusKsuNode::readVariable(IVariable *variable, INode::OperationTimingType timingType)
{
    if (!variables().contains(variable))
        return false;

    ModbusVariable *mbVariable = qobject_cast<ModbusVariable *>(variable);

    QByteArray request = readVariableBytes(mbVariable);
    SharedDataTaskPtr task = std::make_shared<ReadDataTask>(request, timeout());
    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);
    if (timingType == OperationTimingType::Sync) {
        QByteArray response = emit handleTaskSync(request, timeout());

        bool result = parseResponse(task, response);
        ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Read, request, response, mbVariable->alias(), mbVariable->value().toString(), QString::number(result));
        if (result)
            LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
        else
            LoggerTaskManager2::getInstance()->log("Устройство не отвечает", LoggerTaskManager2::Error);
        return result;

        // TODO: запись в архив п. 5.7.1
        // tx = tx
        // rx = rx
    } else {
        emit handleTaskAsync(task);
    }

    return true;
}

bool ModbusKsuNode::writeVariable(IVariable *variable, const QVariant &writedValue, INode::OperationTimingType timingType)
{
    if (!variables().contains(variable))
        return false;

    ModbusVariable *mbVariable = qobject_cast<ModbusVariable *>(variable);

    auto writeDataPtr = variable->writeValueDataForValue(writedValue);
    IVariable::WriteData *writeData = writeDataPtr.get();
    if (!writeData) {
        setLastError(NodeError::wrongWritedValue());
        return false;
    }

    QVariant writedDeviceValue = writeData->writedDeviceValue;

    if (writedValue.isNull() || !writedValue.isValid() || writeData->writedValueBytes.isEmpty()) {
        setLastError(NodeError::wrongWritedValue());
        return false;
    }

    QByteArray request = writeVariableBytes(mbVariable, writedDeviceValue, writeData->writedValueBytes);
    SharedDataTaskPtr task = std::make_shared<WriteDataTask>(variable, writedDeviceValue, request, timeout());
    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);
    if (timingType == OperationTimingType::Sync) {
        QByteArray response = emit handleTaskSync(request, timeout());
        bool result = parseResponse(task, response);
        ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Write, request, response, mbVariable->alias(), mbVariable->value().toString(), QString::number(result));
        if (result)
            LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
        else
            LoggerTaskManager2::getInstance()->log("Устройство не отвечает", LoggerTaskManager2::Error);
        return result;

        // TODO: запись в архив п. 5.7.1
        // tx = tx
        // rx = rx
    } else {
        emit handleTaskAsync(task);
    }

    return true;
}

ModbusVariable *ModbusKsuNode::variableByAddress(quint16 address)
{
    return m_variablesMap.value(address, nullptr);
}

QList<ModbusVariable *> ModbusKsuNode::commonVariables() const
{
    QList<ModbusVariable *> variablesList;
    for (auto variable : m_variablesMap) {
        if (variable)
            variablesList.append(variable);
    }
    return variablesList;
}

IModbusKsuArchive *ModbusKsuNode::readArchive(IModbusKsuArchive::ModbusKsuArchiveType type, bool *ok)
{
    QByteArray request = readArchiveRecordBytes(type);
    SharedDataTaskPtr task = std::make_shared<ReadDataTask>(request, timeout());

    QByteArray response = emit handleTaskSync(request, timeout());

    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);

    IModbusKsuArchive *archiveRecord = nullptr;
    bool result = parseResponse(task, response);

    if (result) {
        archiveRecord = parseArchiveResponse(request, response, type);
        LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
    } else {
        LoggerTaskManager2::getInstance()->log("Устройство не отвечает", LoggerTaskManager2::Error);
    }

    if (ok) {
        *ok = result;
    }

    return archiveRecord;
}

bool ModbusKsuNode::moveArchiveRecordToZeroPosition(IModbusKsuArchive::ModbusKsuArchiveType type)
{
    return moveArchiveRecordToNPosition(0, type);
}

bool ModbusKsuNode::moveArchiveRecordToNextPosition(IModbusKsuArchive::ModbusKsuArchiveType type)
{
    return moveArchiveRecordToNPosition(1, type);
}

bool ModbusKsuNode::moveArchiveRecordToNPosition(quint16 n, IModbusKsuArchive::ModbusKsuArchiveType type)
{
    QByteArray request = moveArchiveRecordToNPositionBytes(n, type);
    SharedDataTaskPtr task = std::make_shared<ReadDataTask>(request, timeout());
    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);
    QByteArray response = emit handleTaskSync(request, timeout());
    IModbusKsuArchive *archiveRecord = nullptr;
    bool result = parseResponse(task, response);
    if (result)
        LoggerTaskManager2::getInstance()->log(response.toHex(), LoggerTaskManager2::Rx);
    else
        LoggerTaskManager2::getInstance()->log("Устройство не отвечает", LoggerTaskManager2::Error);
    return result;
}

uint16_t ModbusKsuNode::readPassword(ModbusKsuPasswordLevel level, bool *ok)
{
    QByteArray request = readPasswordBytes(level);
    SharedDataTaskPtr task = std::make_shared<ReadDataTask>(request, timeout());

    QByteArray response = emit handleTaskSync(request, timeout());

    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);

    uint16_t password = 0;
    bool result = parseReadPasswordResponse(request, response, &password);
    if (result) {
        LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Rx);
    } else {
        LoggerTaskManager2::getInstance()->log(lastError().description(), LoggerTaskManager2::Error);
    }

    if (ok) {
        *ok = result;
    }

    return password;
}

bool ModbusKsuNode::writePassword(ModbusKsuPasswordLevel level, uint16_t password)
{
    QByteArray request = writePasswordBytes(level, password);
    SharedDataTaskPtr task = std::make_shared<ReadDataTask>(request, timeout());

    QByteArray response = emit handleTaskSync(request, timeout());

    LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Tx);

    bool result = parseWritePasswordResponse(request, response);

    if (result) {
        LoggerTaskManager2::getInstance()->log(request.toHex(), LoggerTaskManager2::Rx);
    } else {
        LoggerTaskManager2::getInstance()->log(lastError().description(), LoggerTaskManager2::Error);
    }

    return result;
}

QString ModbusKsuNode::eventClassName(int code) const
{
    QString name;
    if (m_archiveEventClasses.contains(code)) {
        name = m_archiveEventClasses.value(code);
    } else {
        name = QStringLiteral("Неверное значение");
    }

    return QString("%1 (%2)").arg(code).arg(name);
}

QString ModbusKsuNode::eventCodeName(int code) const
{
    QString name;
    if (m_archiveEventCodes.contains(code)) {
        name = m_archiveEventCodes.value(code);
    } else {
        name = QStringLiteral("Неверное значение");
    }

    return QString("%1 (%2)").arg(code).arg(name);
}

QString ModbusKsuNode::kindOfLaunchName(int code) const
{
    QString name;
    if (m_kindOfLaunchCodes.contains(code)) {
        name = m_kindOfLaunchCodes.value(code);
    } else {
        name = QStringLiteral("Неверное значение");
    }

    return QString("%1 (%2)").arg(code).arg(name);
}

QString ModbusKsuNode::stopReasonName(int code) const
{
    QString name;
    if (m_stopReasonCodes.contains(code)) {
        name = m_stopReasonCodes.value(code);
    } else {
        name = QStringLiteral("Неверное значение");
    }

    return QString("%1 (%2)").arg(code).arg(name);
}

void ModbusKsuNode::loadSoftwareVersion()
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
    if (!protocolType || !ok)
        return;

    int protocolCode = query.value(QStringLiteral("comProtocolCode")).toInt(&ok);
    if (ok) {
        setupKindOfLaunch(protocolCode);
        setupStopReasonCodes(protocolCode);
        setupModbusArchivesEventClasses(protocolCode);
        setupModbusArchivesEventCodes(protocolCode);
    }

    QString parametrsConfigFileName = query.value(QStringLiteral("parametrsConfigPath")).toString();
    QDir parametrsConfigDir = QDir(DBConfig::ksuParametrsFolderPath());
    QString parametrsConfigPath = parametrsConfigDir.absoluteFilePath(parametrsConfigFileName);

    loadVariables(parametrsConfigPath);
    //    setupKsu0x20Variables(parametrsConfigPath);

    //    QList<IVariable *> variables;

    //    for (auto var : m_ksu0x10Variables) {
    //        variables.append(var);
    //    }
    //    for (auto var : m_ksu0x20Variables) {
    //        variables.append(var);
    //    }
    //    appendVariables(variables);
}

void ModbusKsuNode::loadVariables(const QString &variablesConfigPath)
{
    QFile configFile(variablesConfigPath);
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
    traverseXMLVariables(xmlConfig);
}

void ModbusKsuNode::traverseXMLVariables(const QDomNode &domNode)
{
    QDomNode node = domNode.firstChild();

    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement element = node.toElement();
            if (element.tagName() == SapksuModbusParametrsMapNodeTag) {
                QDomNodeList parametrsList = element.childNodes();
                int count = parametrsList.count();
                for (int i = 0; i < count; i++) {
                    QDomNode variableNode = parametrsList.at(i);
                    if (variableNode.isNull() || !variableNode.isElement()) continue;

                    QDomElement variableElement = variableNode.toElement();
                    if (variableElement.tagName() != VariableNodeTagName) continue;

                    if (variableElement.attribute(QStringLiteral("id")) != VariableNodeElementId) continue;

                    ModbusVariable *variable = parseVariable(variableElement);
                    if (!variable) continue;

#ifdef QT_DEBUG
                    if (m_variablesMap.contains(variable->address())) {
                        qDebug() << "!!! Попытка создать новую переменную с идентичным адресом" << variable->address();
                        qDebug() << "Переменная" << variable->address() << variable->name();
                        for (auto var : m_variablesMap.values(variable->address())) {
                            qDebug() << "Другие переменные с этим адресом" << var->name();
                        }
                        qDebug() << "\r\n";
                    }
#endif

                    m_variablesMap.insert(variable->address(), variable);
                    appendVariable(variable);
                }
            }
        }

        traverseXMLVariables(node);
        node = node.nextSibling();
    }
}

ModbusVariable *ModbusKsuNode::parseVariable(const QDomElement &domElement)
{
    ModbusVariable *variable = new ModbusVariable(this);
    variable->setParentNode(this);
    variable->setPeriod(period());

    QDomNamedNodeMap elementAttributes = domElement.attributes();
    QStringList commonAttributes = QStringList() << "name"          << "address"
                                                 << "byteOrder"     << "deviceDataType"
                                                 << "description"   << "readable"
                                                 << "writable"      << "period"
                                                 << "measureUnit"   << "serverValueDataType"
                                                 << "minValue"      << "maxValue"
                                                 << "defaultValue"  << "valueStrategyType"
                                                 << "alias"         << "precision"
                                                 << "enabled"       << "permissibleVariation";

    for (QString &attributeName : commonAttributes) {
        if (elementAttributes.contains(attributeName)) {
            QString attributeNodeName = elementAttributes.namedItem(attributeName).nodeName();
            variable->setProperty(attributeNodeName.toLocal8Bit().constData(),
                                  domElement.attribute(attributeNodeName));
        }
    }

    if (VariableValueStrategy *valueStrategy = variable->valueStrategy()) {
        const std::type_info &typeinfo = typeid(*valueStrategy);
        if (typeinfo == typeid(NumberVariableValueStrategy)) {
            NumberVariableValueStrategy *strategy = dynamic_cast<NumberVariableValueStrategy *>(valueStrategy);
            strategy->setCoefficientK(domElement.attribute("coefficientK", QString("1.0")).toDouble());
            strategy->setCoefficientB(domElement.attribute("coefficientB").toDouble());
        } else if (RegisterVariableValueStrategy *strategy = dynamic_cast<RegisterVariableValueStrategy *>(valueStrategy)) {
            strategy->setStartBit(domElement.attribute("startBit").toUShort());
            strategy->setEndBit(domElement.attribute("endBit").toUShort());
            if (typeinfo == typeid(EnumVariableValueStrategy)) {
                QMap<quint64, QString> enumValues;
                QStringList mapStringList = domElement.attribute("enumValues").split(";/", QString::SkipEmptyParts);
                for (const QString &oneValue : mapStringList) {
                    QStringList mapElementStringList = oneValue.split("::");
                    if (mapElementStringList.length() == 2 && !mapElementStringList.at(1).isEmpty()) {
                        bool ok = false;
                        QString keyString = mapElementStringList.at(0);
                        quint64 key = keyString.toULongLong(&ok, 0);
                        if (!ok) continue;
                        enumValues.insert(key, mapElementStringList.at(1));
                    }
                }
                EnumVariableValueStrategy *strategy = dynamic_cast<EnumVariableValueStrategy *>(valueStrategy);
                strategy->setEnumValues(std::move(enumValues));
            }
        } else if (BitsetVariableValueStrategy *strategy = dynamic_cast<BitsetVariableValueStrategy *>(valueStrategy)) {
            QHash<quint64, QString> bitsValues;
            QStringList mapStringList = domElement.attribute("bitsetValues").split(";/", QString::SkipEmptyParts);
            for (const QString &oneValue : mapStringList) {
                QStringList mapElementStringList = oneValue.split("::");
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

    //    if (variable->deviceDataType() == ModbusVariable::ModbusDataType::String) {

    //    }

    return variable;
}

void ModbusKsuNode::setupKindOfLaunch(int comProtocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT code, name FROM `KindOfLaunch` "
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
        m_kindOfLaunchCodes.insert(code, name);
    }
}

void ModbusKsuNode::setupStopReasonCodes(int comProtocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT code, name FROM `StopReasonCodes` "
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
        m_stopReasonCodes.insert(code, name);
    }
}

void ModbusKsuNode::setupModbusArchivesEventClasses(int comProtocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT code, name FROM `ModbusArchivesEventClasses` "
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
        m_archiveEventClasses.insert(code, name);
    }
}

void ModbusKsuNode::setupModbusArchivesEventCodes(int comProtocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT code, name FROM `ModbusArchivesEventCodes` "
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
        m_archiveEventCodes.insert(code, name);
    }
}

void ModbusKsuNode::writeCallback(IVariable *variable, const QVariant &writedValue, const QByteArray &tx, const QByteArray &rx)
{
    // TODO: запись в архив п. 5.7.1
    // tx = tx
    // rx = rx
}

void ModbusKsuNode::readCallback(IVariable *variable, const QByteArray &tx, const QByteArray &rx)
{
    // TODO: запись в архив п. 5.7.1
    // tx = tx
    // rx = rx
}

QList<SharedPeriodicDataTaskPtr> ModbusKsuNode::makePeriodicReadTasks()
{
    return QList<SharedPeriodicDataTaskPtr>();
}

void ModbusKsuNode::writeValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes)
{
    writeVariable(variable, value, OperationTimingType::Async);
}

void ModbusKsuNode::writeValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes)
{
    writeVariable(variable, value, OperationTimingType::Sync);
}

void ModbusKsuNode::readValueSync(IVariable *variable)
{
    readVariable(variable, OperationTimingType::Sync);
}

IModbusKsuArchive *ModbusKsuNode::parseArchiveResponse(const QByteArray &,
                                                       const QByteArray &rx,
                                                       IModbusKsuArchive::ModbusKsuArchiveType type)
{
    QByteArray bytes = rx;
    bytes.chop(2);
    bytes.remove(0, 3);

    IModbusKsuArchive *archiveRecord = nullptr;
    switch (type) {
        case IModbusKsuArchive::ModbusKsuArchiveType::CurrentParametrsArchive:
            archiveRecord = new ModbusKsuCurrentParametrsArchive(this, bytes, this);
            break;

        case IModbusKsuArchive::ModbusKsuArchiveType::ChangeParametrsArchive:
            archiveRecord = new ModbusKsuChangeParametrsArchive(this, bytes, this);
            break;

        case IModbusKsuArchive::ModbusKsuArchiveType::EventsArchive:
            archiveRecord = new ModbusKsuEventsArchive(this, bytes, this);
            break;

        case IModbusKsuArchive::ModbusKsuArchiveType::LaunchParametrsArchive:
            archiveRecord = new ModbusKsuLaunchParametrsArchive(this, bytes, this);
            break;
    }

    return archiveRecord;
}

bool ModbusKsuNode::parseReadPasswordResponse(const QByteArray &tx, const QByteArray &rx, uint16_t *password)
{
    if (rx.isEmpty()) {
        setLastError(NodeError::noResponseError());
        return false;
    }

    if (!validateResponse(tx, rx)) {
        setLastError(NodeError::packageFormatError());
        return false;
    }

    const QByteArray rxPdu = getModbusPdu(rx);

    quint8 rxFunction = rxPdu[ModbusRequestField::Function];
    bool modbusError = (rxFunction >> 7) & 0x01;

    if (modbusError) {
        ModbusError errorCode = static_cast<ModbusError>(rxPdu[2]);
        setLastError(NodeError::nativeDeviceError(errorCode, protocolErrors().value(errorCode)));
        return false;
    }

    QByteArray registersData(rxPdu);
    registersData.remove(0, 3);

    uint16_t pass = *reinterpret_cast<const uint16_t *>(registersData.constData());

    uint16_t part1 = pass & 0x00FF;
    part1 = part1 << 8;
    uint16_t part2 = pass & 0xFF00;
    part2 = part2 >> 8;
    pass = part1 | part2;

    if (password) {
        *password = pass;
    }
    return true;
}

bool ModbusKsuNode::parseWritePasswordResponse(const QByteArray &tx, const QByteArray &rx)
{
    if (rx.isEmpty()) {
        setLastError(NodeError::noResponseError());
        return false;
    }

    if (!validateResponse(tx, rx)) {
        setLastError(NodeError::packageFormatError());
        return false;
    }

    const QByteArray rxPdu = getModbusPdu(rx);

    quint8 rxFunction = rxPdu[ModbusRequestField::Function];
    bool modbusError = (rxFunction >> 7) & 0x01;

    if (modbusError) {
        ModbusError errorCode = static_cast<ModbusError>(rxPdu[2]);
        setLastError(NodeError::nativeDeviceError(errorCode, protocolErrors().value(errorCode)));
        return false;
    }

    return true;
}

QByteArray ModbusKsuNode::moveArchiveRecordToZeroPositionBytes(IModbusKsuArchive::ModbusKsuArchiveType archiveType) const
{
    return moveArchiveRecordToNPositionBytes(0, archiveType);
}

QByteArray ModbusKsuNode::moveArchiveRecordToNextPositionBytes(IModbusKsuArchive::ModbusKsuArchiveType archiveType) const
{
    return moveArchiveRecordToNPositionBytes(1, archiveType);
}

QByteArray ModbusKsuNode::moveArchiveRecordToNPositionBytes(quint16 n, IModbusKsuArchive::ModbusKsuArchiveType archiveType) const
{
    quint16 startRegisterAddress = IModbusKsuArchive::moveArchiveRecordRegisterAddress(archiveType);
    return writeMultipleRegistersBytes(ModbusNode::WriteMultipleRegisters, startRegisterAddress, n);
}

QByteArray ModbusKsuNode::readArchiveRecordBytes(IModbusKsuArchive::ModbusKsuArchiveType archiveType) const
{
    quint16 startRegisterAddress = IModbusKsuArchive::readArchiveStartRegisterAddress(archiveType);
    quint16 registersCount = IModbusKsuArchive::registersCount(archiveType);

    return readRegistersRequestBytes(ModbusNode::ReadHoldingRegisters, startRegisterAddress, registersCount);
}

QByteArray ModbusKsuNode::readPasswordBytes(ModbusKsuPasswordLevel level) const
{
    return readRegistersRequestBytes(MODBUS_KSU_READ_PASSWORD, level, 1);
}

QByteArray ModbusKsuNode::writePasswordBytes(ModbusKsuPasswordLevel level, uint16_t password) const
{
    return writeMultipleRegistersBytes(MODBUS_KSU_WRITE_PASSWORD, level, password);
}
