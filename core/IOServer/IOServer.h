#ifndef IOSERVER_H
#define IOSERVER_H

#include "Common/PrefixHeader.h"
#include "Logger/Logger2/LoggerTaskManager2.h"

class QDomNode;
class DomItem;

class INode;
class IVariable;

class IConnection;
class KsuSerialPortConnection;

class TaskManager;
class TestHistoryReport;

class IOServer final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isStarted READ isStarted)

public:
    enum class PollingState {
        Stoped,
        Pooling,
        Stoping
    };
    Q_ENUM(PollingState)

    static IOServer *instance();
    static void createIOServer();
    static void createIOServer(int ksuSoftwareVersionId, int ksuAddress);
    static void createIOServer(const QString &configPath);
    static void deleteIOServer();

    inline bool isStarted() const { return m_serverPollingState == PollingState::Pooling; }
    inline bool isStoped() const { return m_serverPollingState == PollingState::Stoped; }

    inline const QList<INode *> getNodes() const { return m_nodes; }
    //    inline const QList<IConnection *> getConnections() const { return m_connections; }

    QDomDocument getXmlConfig() const;

    DomItem *rootItem() const;

    inline IVariable *variableByAlias(const QString &alias) const
    {
        QStringList ignoredAliases {"ignored", "aliases", "list"};

        if (!m_variablesAliases.contains(alias) && !ignoredAliases.contains(alias)) {

            LoggerTaskManager2::getInstance()->
                    log(QString(" Вызываемый Alias: \"%1\" не используется с данным типом контроллера").arg(alias),
                        LoggerTaskManager2::MessageType::Info);
            return nullptr;
        }

        return m_variablesAliases.value(alias);
    }

    inline KsuSerialPortConnection *ksuConnection() const { return m_ksuSerialPortConnection; }
    inline INode *ksuNode() const { return m_ksuNode; }

    TestHistoryReport *getTestHistoryReport() const;

    bool getPauseTestStopButton() const;
    void setPauseTestStopButton(bool value);

signals:
    void stopKsu(QPrivateSignal);
    void stopAll(QPrivateSignal);
    void stateChanged();
    void configurationLoaded();
    void updatePortSettings();
    void pauseStateChanged(bool value);

public slots:
    //Не вызывать при включенном сервере
    //Либо добавить в нижеперечисленные методы соответствующие защиты
    void addConnection(IConnection *const connection);
    void removeConnection(IConnection *const connection);

    void addNode(INode *const node);
    void removeNode(INode *const node);

    void addVariable(INode *const node, IVariable *const variable);
    void removeVariable(INode *const node, IVariable *const variable);

    void selectData(int ksuSoftwareVersionId);

    void abortButtonOnPCPressed();
    void abortButtonOnStendPressed();

    //private slots:
    //    void taskManagerDestroyed();
    //    void appAboutToQuit();

private:
    explicit IOServer();
    explicit IOServer(int ksuSoftwareVersionId, int ksuAddress);
    explicit IOServer(const QString &configPath);
    ~IOServer();

    void start();
    bool startKsuPolling();
    void stop();

    void initializeEquipment();


    bool loadConfiguration(const QString &path);
    void variableChangingObserver(const QStringList &aliasList);

    // Обработчик отслеживаемых на изменение переменных
    void handleObserver(IVariable *variable);

    //TODO: времянка, отрефакторить
    //CRITICAL: Говнокод данного метода процветает и продолжает расти.
    void traverseXmlNodes(const QDomNode &node,
                          DomItem *parentDomItem = Q_NULLPTR,
                          INode *parentNode = Q_NULLPTR,
                          IConnection *parentConnection = Q_NULLPTR);

    QList<INode *> searchNodes(IConnection *connections, QList<INode *> nodes);

    void parseXmlInitializationCommands(const QDomElement &element);

    //Это отладочная информация, потом можно будет убрать.
    //Проверяется наличие у объекта (например, INode или IConnection) динамических свойств
    //Т.е. свойств, которые не были определены в классе
    void checkProperties(QObject *object);

    bool pauseTestStopButton = false;

private:
    static IOServer *m_IOServerInstance;

#ifdef SAP_KSU
    QStringList observedVariables{"$SAP_ABORT_BUTTON_PRESSED", "$SAP_ABORT_BUTTON", "$A23_B0", "$SAP_PAUSE_CONTINUE"};
#endif
#ifdef SAP_SU
    QStringList observedVariables{"$SAP_PAUSE", "$SAP_CONTINUE", "$SAP_ABORT_BUTTON_PRESSED"};
#endif
#ifdef MECHOS
    QStringList observedVariables{"$SAP_PAUSE", "$SAP_CONTINUE", "$SAP_ABORT_BUTTON_PRESSED"};
#endif

    QFile m_configFile;
    QDomDocument m_xmlConfig;
    DomItem *m_rootItem;

    QList<INode *> m_nodes;
    QList<IConnection *> m_connections;
    QList<TaskManager *> m_taskManagers;

    INode *m_ksuNode = nullptr;
    KsuSerialPortConnection *m_ksuSerialPortConnection = nullptr;
    TaskManager *m_ksuTaskManager = nullptr;

    PollingState m_ksuPollingState = PollingState::Stoped;
    PollingState m_serverPollingState = PollingState::Stoped;

    QHash<QString, IVariable *> m_variablesAliases;

    QSqlQueryModel *m_dataSelection = nullptr;

    TestHistoryReport *m_testHistoryReport = nullptr;

    struct InitializationCommand {
        QString variableAlias;
        QVariant value;
        bool async = false;
    };
    QList<InitializationCommand *> m_initializationCommandsList;

    Q_DISABLE_COPY(IOServer)
};




#endif // IOSERVER_H
