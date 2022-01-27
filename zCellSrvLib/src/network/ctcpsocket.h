#ifndef CTCPSOCKET_H
#define CTCPSOCKET_H

#include "zCellSrvLib_global.h"

class QTcpSocket;

class ZCELLSRVLIB_EXPORT CTcpSocket
{
public:
    CTcpSocket();

    void connetTo(const char *address, const uint16_t &port);

    virtual ~CTcpSocket();
private:
    QTcpSocket *m_socket = nullptr;
};

#endif // CTCPSOCKET_H
