#include "dataserver.h"
#include "dataserver_p.h"

DataServer::DataServer()
    :d_ptr(new DataServerPrivate)
{
    //    Q_D(DataServer);
    //    d->q_ptr = this;
}

DataServer::~DataServer()
{

}

DataServer::DataServer(DataServerPrivate &dd)
    :d_ptr(&dd)
{
    //    Q_D(DataServer);
    //    d->q_ptr = this;

}
