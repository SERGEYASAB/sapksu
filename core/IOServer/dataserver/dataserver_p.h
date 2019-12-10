#ifndef DATASERVER_P_H
#define DATASERVER_P_H

#include "dataserver.h"

#include <QSharedPointer>

class IDeviceVariable {

private:
    QVariant value;

public:
    QVariant getValue() const;
};

class DataServerPrivate
{
    //Q_DECLARE_PUBLIC(DataServer)

public:
    explicit DataServerPrivate();
    virtual ~DataServerPrivate();

    //DataServer *q_ptr;

private:
    QHash<QString, QSharedPointer<IDeviceVariable>> m_deviceVariables;
};

#endif // DATASERVER_P_H
