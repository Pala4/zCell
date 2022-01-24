#ifndef CTCPSERVER_H
#define CTCPSERVER_H

#include "zCellSrvLib_global.h"

#include<QtNetwork/QTcpServer>

class ZCELLSRVLIB_EXPORT CTCPServer: public QTcpServer
{
    Q_OBJECT
public:
    explicit CTCPServer(QObject *parent = nullptr);
};

#endif // CTCPSERVER_H
