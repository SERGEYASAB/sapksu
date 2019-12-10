#ifndef KSU735NODE_H
#define KSU735NODE_H

#include "Common/PrefixHeader.h"
#include "INode.h"

#include "Ksu735NodeArchive.h"

class Ksu735Variable;
class Ksu735NodeArchive;

class Ksu735Node : public INode
{
    Q_OBJECT

    Q_PROPERTY(quint8 address READ address WRITE setAddress)

public:
    explicit Ksu735Node(int ksuSoftwareVersion, QObject *parent = nullptr);
    Ksu735Node(int ksuSoftwareVersion, int ksuAddress, QObject *parent = nullptr);
    ~Ksu735Node();

    inline quint8 address() const noexcept { return m_address; }
    inline void setAddress(quint8 address) noexcept { m_address = address; }

    inline const Ksu735Variable *statusWordVariable() const;
    inline const Ksu735Variable *stopReasonVariable() const;

    inline QString stopReasonByCode(quint64 code) const;

    inline Ksu735Variable *variableByAddress(quint32 address) const;

public:
    void readAllVariablesAsync();
    bool readVariable(IVariable *variable, OperationTimingType timingType);
    bool readVariables(const QList<IVariable *> &variables, OperationTimingType timingType);

    bool writeVariable(IVariable *variable, const QVariant &writedValue, OperationTimingType timingType);
    bool writeVariables(const QList<QPair<IVariable *, QVariant>> &writedValuesData, OperationTimingType timingType);

    bool readKsu0x10(OperationTimingType timingType);
    Ksu735NodeArchive *readArchive(quint8 offset, bool *ok = nullptr);
    QList<Ksu735NodeArchive *> readArchive(quint8 count, quint8 offset, bool *ok = nullptr);

public:
    inline QList<Ksu735Variable *> ksu0x10Variables() const;
    inline QList<Ksu735Variable *> ksu0x20Variables() const;

protected slots:
    bool parseResponse(SharedDataTaskPtr task, const QByteArray &rx) override;

    void writeValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void writeValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void readValueSync(IVariable *variable) override;

protected: /**< INode interface */
    QList<SharedPeriodicDataTaskPtr> makePeriodicReadTasks() override;


protected: /**< парсинг ответов */
    void parse0x10Response(const QByteArray &tx, const QByteArray &rx);
    void parse0x20Response(const QByteArray &tx, const QByteArray &rx);
    QList<Ksu735NodeArchive *> parse0x30Response(const QByteArray &tx, const QByteArray &rx);

protected: /**< формирование запросов */
    QByteArray readKsu0x10ParametersBytes() const;
    QByteArray readKsu0x20VariableBytes(Ksu735Variable *variable) const;
    QByteArray readKsu0x20TenParametersBytes(const QList<Ksu735Variable *> &parameters) const;
    QByteArrayList readKsu0x20ParametersBytes() const;
    QByteArrayList readKsu0x20ParametersBytes(const QList<Ksu735Variable *> &parameters) const;

    QByteArray readKsu0x30ArchiveBytes(quint8 count, quint8 offset) const;

    QByteArray writeKsu0x20VariableBytes(const QList<QPair<Ksu735Variable *, QByteArray>> &writedValuesData) const;

    QList<Ksu735Variable *> variablesByTx(const QByteArray &tx) const;
    void setVariablesError(const QList<Ksu735Variable *> &variables, const NodeError &error);

private:
    bool read0x10Variables(OperationTimingType timingType);
    bool read0x20Variables(const QList<Ksu735Variable *> variables, OperationTimingType timingType);
    bool write0x20Variables(const QList<QPair<Ksu735Variable *, QVariant>> &writedValuesData, OperationTimingType timingType);

private:
    void loadSoftwareVersion();
    void setupProtocolErrors(int comProtocolCode);
    void setupStatusWordVariable(int comProtocolCode);
    void setupStopReasonVariable(int comProtocolCode);
    void setupKsu0x10Variables(int comProtocolCode);
    void setupKsu0x20Variables(const QString &parametrsPath);

    QList<Ksu735Variable *> special0x10VariablesFromVariable(const Ksu735Variable *variable, int comProtocolCode) const;
    Ksu735Variable *createWorkModeVariable(const Ksu735Variable *variable, const QString &name, int comProtocolCode) const;
    Ksu735Variable *createKindOfLaunchVariable(const Ksu735Variable *variable, const QString &name, int comProtocolCode) const;

    void traverse0x20ConfigXml(const QDomNode &domNode);
    QVariant correctedParametrValueFromFormat(SapKsu::Ksu735ParametrFormat format, const QString &value) const noexcept;

private:
    struct VariablesData {
        typedef enum {
            Read,
            Write
        } FunctionType;

        FunctionType type = Read;

        struct VariableOperationData {
            quint32 variable = 0;
            QVariant value;
        };

        QVector<VariableOperationData> data;
    };

    void successReport(SharedDataTaskPtr task, const QByteArray &rx);
    void errorReport(SharedDataTaskPtr task, const QByteArray &rx);
    void report(const QByteArray &tx, const QByteArray &rx, bool result, void *data);
    void writeReport(const QByteArray &tx, const QByteArray &rx, bool result, const VariablesData &variablesData);

private:
    quint8 m_address = 0;
    int m_ksuSoftwareVersion = -1;

    Ksu735Variable *m_statusWordVariable = nullptr;
    Ksu735Variable *m_stopReasonVariable = nullptr;

    QList<Ksu735Variable *> m_ksu0x10Variables;
    QList<Ksu735Variable *> m_ksu0x20Variables;

    QMultiHash<quint32, Ksu735Variable *> m_ksu0x10VariablesHash;
    QHash<quint32, Ksu735Variable *> m_ksu0x20VariablesHash;

    QHash<quint64, QString> m_stopReasonCodes;

    QHash<int, QString> m_protocolErrors;
};

inline const Ksu735Variable *Ksu735Node::statusWordVariable() const
{ return m_statusWordVariable; }

inline const Ksu735Variable *Ksu735Node::stopReasonVariable() const
{ return m_stopReasonVariable; }

QString Ksu735Node::stopReasonByCode(quint64 code) const
{ return m_stopReasonCodes.value(code); }

Ksu735Variable *Ksu735Node::variableByAddress(quint32 address) const
{ return m_ksu0x20VariablesHash.value(address, nullptr); }

inline QList<Ksu735Variable *> Ksu735Node::ksu0x10Variables() const
{ return m_ksu0x10Variables; }

inline QList<Ksu735Variable *> Ksu735Node::ksu0x20Variables() const
{ return m_ksu0x20Variables; }

#endif // KSU735NODE_H
