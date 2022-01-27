#include "ctcpsocket.h"

#include <QtNetwork/QTcpSocket>

CTcpSocket::CTcpSocket()
{
}

void CTcpSocket::connetTo(const char *address, const uint16_t &port)
{
    QString addr = QString(address);
    QHostAddress ha(addr);
    m_socket = new QTcpSocket();
    m_socket->connectToHost(ha, port);
}

CTcpSocket::~CTcpSocket()
{
    if (m_socket != nullptr)
        delete m_socket;
}
