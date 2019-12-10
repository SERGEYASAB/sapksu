#include "IOServer.h"
#include "DBConfig/DBConfig.h"
#include "Logger/LoggerTaskManager.h"

#include "IOServerUI/DomItem.h"
#include "AppSettings/AppSettings.h"

#include "IOServer/nsTask/TaskManager.h"

#include "IOServer/nsConnection/IConnection.h"
#include "IOServer/nsConnection/SerialPortConnection.h"
#include "IOServer/nsConnection/KsuSerialPortConnection.h"
#include "IOServer/nsConnection/TcpConnection.h"

#include "IOServer/nsNode/INode.h"
#include "IOServer/nsNode/ModbusNode.h"
#include "IOServer/nsNode/ModbusRtuNode.h"
#include "IOServer/nsNode/ModbusTcpNode.h"
#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsNode/ModbusKsuNode.h"

#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/IDeviceVariableValue.h"
#include "IOServer/nsVariable/ModbusVariable.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/NumberVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/EnumVariableValueStrategy.h"
#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/BitsetVariableValueStrategy.h"

#include "StartWindow/SystemTrayIcon.h"

#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "Logger/Logger2/LoggerTaskManager2.h"
#include "ReportManager/FaultReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

static const QString IOServerConfigurationInitializationSection = "InitializationSection";
static const QString IOServerConfigurationInitializationCommand = "InitializationCommand";

IOServer *IOServer::m_IOServerInstance = nullptr;

IOServer::IOServer(int ksuSoftwareVersionId, int ksuAddress)
{
    m_dataSelection = new QSqlQueryModel(this);
    selectData(ksuSoftwareVersionId);

    m_ksuSerialPortConnection = new KsuSerialPortConnection();
    connect(this, &IOServer::updatePortSettings, m_ksuSerialPortConnection, &KsuSerialPortConnection::updatePortSettingsFromAppSettings);
    emit updatePortSettings();

    QSqlRecord record = m_dataSelection->record(0);

    if (record.value(QStringLiteral("protocol")).toInt() == 0) {
        m_ksuNode = new Ksu735Node(ksuSoftwareVersionId, ksuAddress);
    } else {
        m_ksuNode = new ModbusKsuNode(ksuSoftwareVersionId, ksuAddress);
    }

    // Значение по умолчанию между посылками
    m_ksuNode->setTimeout(300);

    loadConfiguration(DBConfig::sapParametrsFolderPath() +
                      QDir::separator() +
                      record.value(QStringLiteral("path")).toString());

    m_testHistoryReport = new TestHistoryReport(this);

    // Заполнение данными - тип Ксу и версия ПО Ксу - класса TestHistoryReport

    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("SELECT KsuTypes.ksuName, KsuSoftwareVersions.softwareVersion "
                                "FROM KsuSoftwareVersions JOIN KsuTypes ON KsuSoftwareVersions.ksuTypeId = KsuTypes.id "
                                "WHERE KsuSoftwareVersions.id = %1").arg(ksuSoftwareVersionId);

    QSqlQuery query(sqlString, db);
    db.transaction();
    query.exec();
    db.commit();

    if (!query.first()) {
        ErrorClassifier::instance()->setErrorDescription(500, QString("Отсутствует результат по запросу: %1").arg(sqlString));
        ErrorClassifier::instance()->showError(500);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(500) + query.lastError().databaseText());
    }

    m_testHistoryReport->setKsuType(query.value(QStringLiteral("ksuName")).toString());
    m_testHistoryReport->setKsuSoftwareVersion(query.value(QStringLiteral("softwareVersion")).toString());

    start();
    startKsuPolling();
    variableChangingObserver(observedVariables);

    initializeEquipment();
}

IOServer::IOServer(const QString &configPath)
{
    loadConfiguration(configPath);

    m_testHistoryReport = new TestHistoryReport(this);
    m_testHistoryReport->setKsuType(QString("-"));
    m_testHistoryReport->setKsuSoftwareVersion(QString("-"));

    start();
    variableChangingObserver(observedVariables);

    initializeEquipment();
}

IOServer::IOServer()
{
#ifdef SAP_KSU
    loadConfiguration(":/DBConfig/SAPParametrs/KSU_Default_Template.xml");
#endif

#ifdef SAP_SU
    loadConfiguration(":/DBConfig/SAPParametrs/SAPSU_Default_Template.xml");
#endif

#ifdef MECHOS
    loadConfiguration(":/DBConfig/SAPParametrs/Mechos_Default_Template.xml");
#endif

    m_testHistoryReport = new TestHistoryReport(this);
    m_testHistoryReport->setKsuType(QString("-"));
    m_testHistoryReport->setKsuSoftwareVersion(QString("-"));


    start();
    variableChangingObserver(observedVariables);

    initializeEquipment();
}

IOServer *IOServer::instance()
{
    return m_IOServerInstance;
}

//TODO: подумать над удалением сервера при создании
void IOServer::createIOServer()
{
    if (m_IOServerInstance != nullptr)
        deleteIOServer();

    m_IOServerInstance = new IOServer();
}

void IOServer::createIOServer(int ksuSoftwareVersionId, int ksuAddress)
{
    if (m_IOServerInstance != nullptr)
        deleteIOServer();

    m_IOServerInstance = new IOServer(ksuSoftwareVersionId, ksuAddress);
}

void IOServer::createIOServer(const QString &configPath)
{
    if (m_IOServerInstance != nullptr)
        deleteIOServer();

    m_IOServerInstance = new IOServer(configPath);
}

// Объективно стало только ХУЖЕ был один метод - СБРОС ВСЕГО - теперь надо думать какой вызывать, и в деструкторе все равно
// Происходит лишнее переключение

IOServer::~IOServer()
{
    abortButtonOnPCPressed();
    stop();
    qDeleteAll(m_nodes);
    qDeleteAll(m_initializationCommandsList);
    //    qDeleteAll(m_connections);

    //    delete m_ksuSerialPortConnection;
}


void IOServer::deleteIOServer()
{
    delete m_IOServerInstance;
    m_IOServerInstance = nullptr;
}

bool IOServer::loadConfiguration(const QString &path)
{
    if (m_serverPollingState == PollingState::Pooling)
        return false;

    m_configFile.setFileName(path);
    if (!m_configFile.open(QIODevice::ReadOnly)) {
        qCritical() << tr("Не удается открыть файл конфигурации") << "(ошибка:" << m_configFile.errorString() << ")";
        FaultReport::instance()->write("Ошибка IOServer " + tr("Не удается открыть файл конфигурации"));
        return false;
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    if (!m_xmlConfig.setContent(&m_configFile, &errorMsg, &errorLine, &errorColumn)) {
        qCritical() << tr("Конфигурационный файл поврежден") << errorMsg << errorLine << errorColumn;
        m_configFile.close();
        return false;
    }
    m_configFile.close();

    //TODO: допереписать нормально парсинг или удаление нодов (сначала они присваиваются - потом удаляются)
    for (INode *node : m_nodes) {
        node->deleteLater();
    }
    m_nodes.clear();

    for (IConnection *connection : m_connections) {
        connection->deleteLater();
    }
    m_connections.clear();

    traverseXmlNodes(m_xmlConfig);

    addNode(m_ksuNode);

    emit configurationLoaded();
    return true;
}

void IOServer::variableChangingObserver(const QStringList &aliasList)
{
    for (auto alias : aliasList) {
        IVariable *variable = variableByAlias(alias);
        if (variable)
            connect(variable, &IVariable::valueChanged, this, &IOServer::handleObserver);
    }
}

void IOServer::handleObserver(IVariable *variable)
{
    if (variable->alias() == "$SAP_ABORT_BUTTON_PRESSED") {
        if (variable->value().toBool() == true) {
            abortButtonOnStendPressed();

            if (pauseTestStopButton) {
                setPauseTestStopButton(false);
            } else {
                setPauseTestStopButton(true);
            }

            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle("Завершение");
            msgBox.setText(tr("НАЖАТА КНОПКА АВАРИЙНОГО ПРЕРЫВАНИЯ"));
            msgBox.setInformativeText("Программа будет перезапущена");
            msgBox.setMinimumWidth(400);

            FaultReport::instance()->write("НАЖАТА КНОПКА ПРЕРВАТЬ");

            QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
            QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
            if (layout)
                layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

            // restart:
            qApp->quit(); // SAP_ABORT_BUTTON
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());

        }
    } else if (variable->alias() == "$SAP_ABORT_BUTTON") {
    } else if (variable->alias() == "$A23_B0") {
        if (variable->value().toBool() == true) {

            abortButtonOnPCPressed();

            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle("реле KV1");
            msgBox.setText(tr("Повторите попытку позже"));
            msgBox.setInformativeText("ЗАЩИТА ОТ ПЕРЕНАПРЯЖЕНИЯ");
            msgBox.setMinimumWidth(400);

            QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
            QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
            if (layout)
                layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        }
    } else if (variable->alias() == "$SAP_PAUSE_CONTINUE") {

        if (variable->value().toBool() == true) {

            if (pauseTestStopButton) {

                setPauseTestStopButton(false);

            } else {

                setPauseTestStopButton(true);

                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle("Команда управления со стенда");
                msgBox.setText(tr("Нажата кнопка пауза"));
                msgBox.setInformativeText("Выполнение теста будет приостановлено со следующего шага");
                msgBox.setMinimumWidth(400);

                QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
                QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
                if (layout)
                    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

                msgBox.setDefaultButton(QMessageBox::Ok);
                msgBox.exec();

            }

        }

    }

#ifdef SAP_SU

    if (variable->alias() == "$SAP_PAUSE") {
        if (variable->value().toBool() == true) {

            setPauseTestStopButton(true);

            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle("Команда управления со стенда");
            msgBox.setText(tr("Нажата кнопка пауза"));
            msgBox.setInformativeText("Выполнение теста будет приостановлено со следующего шага");
            msgBox.setMinimumWidth(400);

            QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
            QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
            if (layout)
                layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

        }

    } else if (variable->alias() == "$SAP_CONTINUE") {
        if (variable->value().toBool() == true) {

            setPauseTestStopButton(false);

        }
    }

#endif

#ifdef MECHOS

    if (variable->alias() == "$SAP_PAUSE") {
        if (variable->value().toBool() == true) {

            setPauseTestStopButton(true);

            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle("Команда управления со стенда");
            msgBox.setText(tr("Нажата кнопка пауза"));
            msgBox.setInformativeText("Выполнение теста будет приостановлено со следующего шага");
            msgBox.setMinimumWidth(400);

            QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
            QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
            if (layout)
                layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

        }

    } else if (variable->alias() == "$SAP_CONTINUE") {
        if (variable->value().toBool() == true) {

            setPauseTestStopButton(false);

        }
    }

#endif

}

void IOServer::start()
{
    if (m_serverPollingState != PollingState::Stoped) {
        return;
    }

    for (auto connection : m_connections) {
        QList<INode *> connectionNodes = searchNodes(connection, m_nodes);
        for (auto node : connectionNodes) {
            for (auto variable : node->variables())
                if (!variable->alias().isEmpty())
                    m_variablesAliases.insert(variable->alias(), variable);
        }

        QThread *thread = new QThread();
        thread->setObjectName(connection->name() + "_Thread");

        TaskManager *taskManager = new TaskManager(connection, connectionNodes, thread);
        m_taskManagers.append(taskManager);

        taskManager->moveToThread(thread);
        connection->moveToThread(thread);

        connect(thread, &QThread::started, taskManager, &TaskManager::process);
        connect(taskManager, &TaskManager::finished, thread, &QThread::quit);

        connect(taskManager, &TaskManager::finished, taskManager, &TaskManager::deleteLater);
        connect(taskManager, &TaskManager::finished, connection, &IConnection::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        connect(this, &IOServer::stopAll, taskManager, &TaskManager::stop);
        //        connect(taskManager, &TaskManager::destroyed, this, &IOServer::taskManagerDestroyed);

        thread->start();
    }

    m_serverPollingState = PollingState::Pooling;
    emit stateChanged();
}

bool IOServer::startKsuPolling()
{
    if (m_ksuPollingState != PollingState::Stoped)
        return false;

    if (!m_ksuSerialPortConnection || !m_ksuNode)
        return false;

    for (auto variable : m_ksuNode->variables()) {
        if (!variable->alias().isEmpty())
            m_variablesAliases.insert(variable->alias(), variable);
    }

    QThread *thread = new QThread();
    thread->setObjectName(m_ksuSerialPortConnection->name() + "_Thread");

    TaskManager *taskManager = new TaskManager(m_ksuSerialPortConnection, QList<INode *>() << m_ksuNode, thread);
    m_taskManagers.append(taskManager);

    taskManager->moveToThread(thread);
    m_ksuSerialPortConnection->moveToThread(thread);

    connect(thread, &QThread::started, taskManager, &TaskManager::process);
    connect(taskManager, &TaskManager::finished, thread, &QThread::quit);

    connect(taskManager, &TaskManager::finished, taskManager, &TaskManager::deleteLater);
    connect(taskManager, &TaskManager::finished, m_ksuSerialPortConnection, &IConnection::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    connect(this, &IOServer::stopAll, taskManager, &TaskManager::stop);
    connect(this, &IOServer::stopKsu, taskManager, &TaskManager::stop);
    //    connect(taskManager, &TaskManager::destroyed, this, &IOServer::taskManagerDestroyed);

    thread->start();

    m_ksuTaskManager = taskManager;
    m_ksuPollingState = PollingState::Pooling;

    return true;
}

void IOServer::stop()
{
    m_ksuPollingState = PollingState::Stoping;
    m_serverPollingState = PollingState::Stoping;

    emit stopAll(QPrivateSignal());

    //    for (auto manager : m_taskManagers) {
    //        manager->thread()->wait();
    //        delete manager->thread();
    //        delete manager;
    //    }

    m_taskManagers.clear();
    m_ksuTaskManager = nullptr;

    m_ksuPollingState = PollingState::Stoped;
    m_serverPollingState = PollingState::Stoped;
}

void IOServer::initializeEquipment()
{
    for (auto command : m_initializationCommandsList) {
        IVariable *variable = variableByAlias(command->variableAlias);
        if (!variable)
            continue;

        if (!variable->readValue().isValid()) {

#ifdef QT_NO_DEBUG
            QMessageBox msgBox;
            msgBox.setWindowTitle("Ошибка связи с оборудованием стенда");
            msgBox.setText(QString("КРИТИЧЕСКАЯ ОШИБКА \n"
                                   "Нет связи с узлом %1 \n%2\n"
                                   "При отсутствии связи с оборудованием стенда работа недопустима\n"
                                   "После нажатие клавиши ОК будет выполнена деинициализация и приложение \nбудет закрыто").
                           arg(variable->parentNode()->name()).
                           arg(variable->parentNode()->description()));
            msgBox.setWindowIcon(QIcon(":/Images/irz_logo.png"));

            FaultReport::instance()->write(QString("Ошибка связи с оборудованием стенда IOServer - Нет связи с узлом %1").arg(variable->parentNode()->name()));

            QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
            QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
            if (layout)
                layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

            msgBox.setMinimumWidth(400);

            if (msgBox.exec() == QMessageBox::Ok) {
                abortButtonOnPCPressed();
                qApp->exit();
                break;
            }


#endif
        }

        if (command->async) {
            variable->writeValue(command->value);
        } else {
            variable->setValue(command->value);
        }
    }
}

void IOServer::addConnection(IConnection *const connection)
{
    if (connection)
        m_connections.append(connection);
}

void IOServer::removeConnection(IConnection *const connection)
{
    if (connection) {
        m_xmlConfig.removeChild((*connection).domElement());
        m_connections.removeOne(connection);
    }
}

void IOServer::addNode(INode *const node)
{
    if (node) {
        m_nodes.append(node);
    }
}

void IOServer::removeNode(INode *const node)
{
    if (node) {
        m_xmlConfig.removeChild(node->getConfigNode());
        m_nodes.removeOne(node);
    }
}

void IOServer::addVariable(INode *const node, IVariable *const variable)
{
    if (node && variable) {
        node->appendVariable(variable);
    }
}

void IOServer::removeVariable(INode * const node, IVariable * const variable)
{
    if (node && variable) {
        m_xmlConfig.removeChild(*variable->getConfigNode());
        node->removeVariable(variable);
    }
}

//void IOServer::taskManagerDestroyed()
//{
//    auto it = m_taskManagers.begin();
//    while (it != m_taskManagers.end()) {
//        if (it->isNull())
//            it = m_taskManagers.erase(it);
//        else
//            ++it;
//    }

//    if (m_ksuTaskManager.isNull()) {
//        m_ksuPollingState = PollingState::Stoped;
//    }

//    if (m_taskManagers.isEmpty()) {
//        m_serverPollingState = PollingState::Stoped;
//        emit stateChanged();
//    }
//}

//void IOServer::appAboutToQuit()
//{
//    emit (QPrivateSignal());
//}

void IOServer::traverseXmlNodes(const QDomNode &node,
                                DomItem *parentDomItem,
                                INode *parentNode,
                                IConnection *parentConnection)
{
    QDomNode domNode = node.firstChild();

    // Значение current параметров определяется по ходу алгоритма и передаются следующему domNode
    IConnection *currentConnection = nullptr;
    INode *currentNode = nullptr;
    DomItem *currentDomItem = nullptr;

    //    DomItem *m_parentDomItem = nullptr;

    while (!domNode.isNull()) {

        if (domNode.isElement()) {

            QDomElement domElement = domNode.toElement();

            if (!domElement.isNull()) {
                auto elementId = domElement.attribute("id");

                if (domElement.tagName() == "IConnection") {

                    IConnection *connection = nullptr;

                    if (elementId == "SerialPortConnection") {
                        connection = new SerialPortConnection(domElement);
                    } else if (elementId == "TcpConnection") {
                        connection = new TcpConnection(domElement);
                    }

                    if (connection) {

                        QDomNamedNodeMap attributes = domElement.attributes();

                        for (int a = 0; a < attributes.count(); a++) {
                            connection->setProperty(attributes.item(a).nodeName().toLocal8Bit().constData(),
                                                    domElement.attribute(attributes.item(a).nodeName()));
                        }

                        DomItem *domItem = new DomItem(connection,
                                                       domElement,
                                                       domElement.childNodes().count(),
                                                       parentDomItem);

                        currentConnection = connection;
                        currentDomItem = domItem;
                        addConnection(connection);
                    }

                } else if (domElement.tagName() == "INode") {

                    INode *node = nullptr;

                    if (elementId == "ModbusTcpNode") {
                        node = new ModbusTcpNode(domElement);
                    } else if (elementId == "ModbusRtuNode") {
                        node = new ModbusRtuNode(domElement);
                    }

                    if (node) {
                        node->setParentNode(parentNode);
                        node->setConnectionName(parentConnection->name());

                        QDomNamedNodeMap attributes = domElement.attributes();

                        for (int a = 0; a < attributes.count(); a++) {
                            node->setProperty(attributes.item(a).nodeName().toLocal8Bit().constData(),
                                              domElement.attribute(attributes.item(a).nodeName()));
                        }

                        if (parentNode == nullptr) {
                            node->setPath(node->name() + "_");
                        } else {
                            node->setPath(parentNode->path() + node->name() + "_");
                        }

                        DomItem *domItem = new DomItem(node,
                                                       domElement,
                                                       domElement.childNodes().count(),
                                                       parentDomItem);

                        currentNode = node;
                        currentConnection = parentConnection;
                        currentDomItem = domItem;
                        addNode(node);
                    }
                } else if (domElement.tagName() == "IVariable") {

                    IVariable *variable = nullptr;
                    QDomNamedNodeMap attributes = domElement.attributes();

                    if (elementId == "ModbusVariable") {
                        variable = new ModbusVariable(parentNode);

                        QStringList modbusAttributes = QStringList() << "address" << "byteOrder" << "deviceDataType";
                        for (QString &attributeName : modbusAttributes) {
                            if (attributes.contains(attributeName)) {
                                QString attributeNodeName = attributes.namedItem(attributeName).nodeName();
                                variable->setProperty(attributeNodeName.toLocal8Bit().constData(),
                                                      domElement.attribute(attributeNodeName));
                            }
                        }
                    }

                    if (variable) {
                        variable->setParentNode(parentNode);
                        variable->setPeriod(parentNode->period());
                        addVariable(parentNode, variable);

                        QStringList commonAttributes = QStringList() << "name"                  << "alias"
                                                                     << "description"           << "readable"
                                                                     << "writable"              << "period"
                                                                     << "measureUnit"           << "precision"
                                                                     << "serverValueDataType"   << "minValue"
                                                                     << "maxValue"              << "valueStrategyType"
                                                                     << "defaultValue";

                        for (QString &attributeName : commonAttributes) {
                            if (attributes.contains(attributeName)) {
                                QString attributeNodeName = attributes.namedItem(attributeName).nodeName();
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

                        DomItem *domItem = new DomItem(variable,
                                                       domElement,
                                                       domElement.childNodes().count(),
                                                       parentDomItem);

                        currentNode = parentNode;
                        currentDomItem = domItem;
                        currentConnection = parentConnection;
                    }

                } else if (domElement.tagName() == "SAPKSU") {
                    m_rootItem = new DomItem(nullptr, domElement, 0, nullptr);
                    currentDomItem = m_rootItem;

                    QDomElement initializationCommandsElement = domElement.firstChildElement(IOServerConfigurationInitializationSection);
                    if (!initializationCommandsElement.isNull()) {
                        parseXmlInitializationCommands(initializationCommandsElement);
                    }
                }

            }
        }

        traverseXmlNodes(domNode, currentDomItem, currentNode, currentConnection);
        domNode = domNode.nextSibling();
    }
}

QList<INode *> IOServer::searchNodes(IConnection *connections, QList<INode *> nodes)
{
    QList<INode *> selectedNodes;
    for (auto i : nodes)
        if (i->connectionName() == connections->name())
            selectedNodes.append(i);
    return selectedNodes;
}

void IOServer::parseXmlInitializationCommands(const QDomElement &element)
{
    QDomNodeList commandList = element.childNodes();
    int count = commandList.count();
    for (int i = 0; i < count; i++) {
        QDomNode commandNode = commandList.at(i);
        if (commandNode.isNull() || !commandNode.isElement()) continue;

        QDomElement commandElement = commandNode.toElement();
        if (commandElement.tagName() != IOServerConfigurationInitializationCommand) continue;

        QString variableAlias = commandElement.attribute(QStringLiteral("variable"));
        QString initialValue = commandElement.attribute(QStringLiteral("value"));
        QString initializationType = commandElement.attribute(QStringLiteral("type"));

        if (variableAlias.isEmpty() || initialValue.isEmpty()) continue;

        InitializationCommand *command = new InitializationCommand();
        command->variableAlias = variableAlias;
        command->value = QVariant::fromValue(initialValue);

        qDebug() << "command->value" << command->value;

        if (initializationType == QStringLiteral("Async"))
            command->async = true;

        m_initializationCommandsList.append(command);
    }
}

void IOServer::setPauseTestStopButton(bool value)
{
    if (pauseTestStopButton == value)
        return;

    pauseTestStopButton = value;

    emit pauseStateChanged(value);
}

bool IOServer::getPauseTestStopButton() const
{
    return pauseTestStopButton;
}

TestHistoryReport *IOServer::getTestHistoryReport() const
{
    return m_testHistoryReport;
}

void IOServer::selectData(int ksuSoftwareVersionId)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString = QString("SELECT sapparametrs.parametrsConfigPath as path, "
                                "KsuSoftwareVersions.protocolType as protocol "
                                "FROM KsuSoftwareVersions "
                                "INNER JOIN KsuTypes ON KsuSoftwareVersions.ksuTypeId = KsuTypes.id "
                                "INNER JOIN SAPParametrs as sapparametrs ON KsuTypes.sapParamId = sapparametrs.id "
                                "WHERE KsuSoftwareVersions.id = %1").arg(ksuSoftwareVersionId);

    QSqlQuery query(sqlString, db);

    m_dataSelection->setQuery(query);
}

void IOServer::abortButtonOnStendPressed()
{
    IVariable *variable = nullptr;
    QStringList aliasList;

#ifdef SAP_KSU
    // После нажатия кнопки "Прервать" на стенде в регистр записывается 1. Сброс осуществляется только клиентом ModbusTCP
    aliasList << "$SAP_ABORT_BUTTON_PRESSED";

    // После сброса питания AFC 200 снимается, но с регистров продолжает читаться 1
    // Добавляем принудительную запись, к сожалению кнопка на шкафу этого сделать не мди .
    aliasList << "$UZ1_CTRL_DRIVE" << "$UZ2_CTRL_DRIVE" << "$UZ3_CTRL_DRIVE";
#endif
#ifdef SAP_SU
    // Внимание! Нельзя нарушать последовательность POWER_SUPPLY и Фазы ABC
    aliasList << "$SAP_POWER_SUPPLY" << "$SAP_PHASE_A_ENABLE" << "$SAP_PHASE_B_ENABLE" << "$SAP_PHASE_C_ENABLE";
    aliasList << "$SAP_POWER_PCH" << "$SAP_POWER_PCH_ENABLE";
#endif
#ifdef MECHOS
    // Внимание! Нельзя нарушать последовательность POWER_SUPPLY и Фазы ABC
    aliasList << "$SAP_POWER_SUPPLY" << "$SAP_PHASE_A_ENABLE" << "$SAP_PHASE_B_ENABLE" << "$SAP_PHASE_C_ENABLE";
    aliasList << "$SAP_POWER_PCH" << "$SAP_POWER_PCH_ENABLE" << "$SAP_KSULINK_ENABLE";
#endif

    for (auto aliasName : aliasList) {

        variable = variableByAlias(aliasName);

        if (!variable)
            continue;

        variable->readValue();
        variable->setValue(0);

    }
}

void IOServer::abortButtonOnPCPressed()
{

#ifdef SAP_KSU

    IVariable *variable = variableByAlias("$SAP_ABORT_BUTTON");
    if (!variable) {
        qDebug() << "Переменная с алиасом $SAP_ABORT_BUTTON не найдена";
        return;
    }

    //variable->readValue();
    variable->setValue(1);

#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS
#endif

    abortButtonOnStendPressed();
}

DomItem *IOServer::rootItem() const
{
    return m_rootItem;
}

QDomDocument IOServer::getXmlConfig() const
{
    return m_xmlConfig;
}



