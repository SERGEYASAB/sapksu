#ifndef NODEERROR_H
#define NODEERROR_H

class NodeError final
{
public:
    inline int code() const { return m_errorCode; }
    inline QString description() const { return m_description; }
    inline bool isDeviceError() const { return m_deviceError; }

    bool operator!=(const NodeError &other) const;
    bool operator==(const NodeError &other) const;

    //static
public:
    static NodeError noError();
    static NodeError noResponseError();
    static NodeError crcError();
    static NodeError packageFormatError();
    static NodeError unknownDeviceError();
    static NodeError wrongWritedValue();
    static NodeError nativeDeviceError(int code, const QString &description);

private:
    NodeError(int code = 0, const QString &description = QString(), bool deviceError = false);

    int m_errorCode = 0;
    QString m_description;
    bool m_deviceError = false;
};

#endif // NODEERROR_H
