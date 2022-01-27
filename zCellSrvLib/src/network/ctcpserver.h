#ifndef CTCPSERVER_H
#define CTCPSERVER_H

#include "zCellSrvLib_global.h"

class QTcpServer;

class ZCELLSRVLIB_EXPORT CTCPServer
{
private:
    QTcpServer *m_tcp_server = nullptr;
public:
    CTCPServer();

    bool start(const char *address, const uint16_t &port);

    virtual ~CTCPServer();
};

#endif // CTCPSERVER_H
