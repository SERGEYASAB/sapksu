#include "NodeError.h"
#include "ReportManager/FaultReport.h"

bool NodeError::operator!=(const NodeError &other) const
{
    return !(*this == other);
}

bool NodeError::operator==(const NodeError &other) const
{
    return code() == other.code() && description() == other.description() && isDeviceError() == other.isDeviceError();
}

NodeError NodeError::noError()
{
    return NodeError();
}

NodeError NodeError::noResponseError()
{
    return NodeError(0x100, "Устройство не отвечает", false);
}

NodeError NodeError::crcError()
{
    return NodeError(0x101, "Ошибка CRC", false);
}

NodeError NodeError::packageFormatError()
{
    return NodeError(0x102, "Неверный формат пакета", false);
}

NodeError NodeError::unknownDeviceError()
{
    return NodeError(0x103, "Неизвестная ошибка на подчиненном устройстве", false);
}

NodeError NodeError::wrongWritedValue()
{
    return NodeError(0x104, "Записываемое значение некорректно", false);
}

NodeError NodeError::nativeDeviceError(int code, const QString &description)
{
    return NodeError(code, description, true);
}

NodeError::NodeError(int code, const QString &description, bool deviceError) :
    m_errorCode(code), m_description(description), m_deviceError(deviceError)
{
    //FaultReport::instance()->write("Ошибка NodeError" + description);
}
