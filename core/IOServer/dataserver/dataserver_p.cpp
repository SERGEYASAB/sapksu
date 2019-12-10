#include "dataserver_p.h"

DataServerPrivate::DataServerPrivate()
{
    QSharedPointer<IDeviceVariable> variable = QSharedPointer<IDeviceVariable>(new IDeviceVariable /* TODO: IDeviceVariableDeleter */);

    m_deviceVariables.insert(QString("#DeviceValueAlias"), variable);
}

DataServerPrivate::~DataServerPrivate()
{


}

QVariant IDeviceVariable::getValue() const
{
    return value;
}
