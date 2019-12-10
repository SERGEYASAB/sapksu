#ifndef DATASERVER_H
#define DATASERVER_H

#include <QScopedPointer>

class DataServerPrivate;
class DataServer
{
    Q_DECLARE_PRIVATE(DataServer)

public:
    explicit DataServer();
    virtual ~DataServer();

protected:
    DataServer(DataServerPrivate &dd);
    QScopedPointer<DataServerPrivate> d_ptr;

private:
    Q_DISABLE_COPY(DataServer)
};

#endif // DATASERVER_H
