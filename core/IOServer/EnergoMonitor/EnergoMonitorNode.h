#ifndef ENERGOMONITORNODE_H
#define ENERGOMONITORNODE_H

#include "Common/PrefixHeader.h"
#include "IOServer/nsNode/INode.h"
#include "IOServer/nsVariable/IVariable.h"

class EnergoMonitorNode : public INode
{
    Q_OBJECT

public:
    EnergoMonitorNode(QObject *parent = Q_NULLPTR);

    // INode interface
protected:
    QList<SharedPeriodicDataTaskPtr> makePeriodicReadTasks() override;

protected slots:
    bool parseResponse(SharedDataTaskPtr task, const QByteArray &rx) override;
    void writeValueAsync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void writeValueSync(IVariable *variable, const QVariant &value, const QByteArray &bytes) override;
    void readValueSync(IVariable *variable) override;
};

#endif // ENERGOMONITORNODE_H
