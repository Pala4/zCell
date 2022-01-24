#include <QCoreApplication>

#include <iostream>

#include "network/ctcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QHostAddress ha("127.0.0.1");
    CTCPServer v;
    v.listen(ha, 5356);
    if (v.isListening()) {
        std::cout << "Server running on address: " <<
                     v.serverAddress().toString().toLatin1().constData() <<
                     " and port: " << v.serverPort() << std::endl;
    }

    return a.exec();
}
