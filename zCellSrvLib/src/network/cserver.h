#ifndef CSERVER_H
#define CSERVER_H

#include <QtNetwork/QTcpServer>

#include "zCellSrvLib_global.h"
#include "cnetapplication.h"

namespace zcell_lib {

class CThreadPool;

class CTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    typedef std::function<void(int)> incoming_connection_handler_t;

    explicit CTcpServer(const incoming_connection_handler_t &incoming_connection_handler,
                        QObject *parent = nullptr);
protected:
    virtual void incomingConnection(qintptr socketDescriptor) override final;
private:
    incoming_connection_handler_t m_incoming_connection_handler = nullptr;
};

class ZCELLSRVLIB_EXPORT CServer : public CNetApplication
{
public:
    CServer();

    bool start(const char *address, const uint16_t &port);

    virtual ~CServer();
protected:
    virtual void process_net_data(const net_data_ptr_t &net_data);
private:
    CTcpServer *m_tcp_server = nullptr;
};

} // namespace zcell_lib

#endif // CSERVER_H
