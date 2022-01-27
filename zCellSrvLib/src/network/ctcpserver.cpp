#include "ctcpserver.h"

#include <QtNetwork/QTcpServer>

#include <iostream>

CTCPServer::CTCPServer()
{
    m_tcp_server = new QTcpServer();
}

bool CTCPServer::start(const char *address, const uint16_t &port)
{
    QString addr = QString(address);
    QHostAddress ha(addr);
    if (m_tcp_server != nullptr)
        return m_tcp_server->listen(ha, port);
    return false;
}

CTCPServer::~CTCPServer()
{
    if (m_tcp_server != nullptr)
        delete m_tcp_server;
}
