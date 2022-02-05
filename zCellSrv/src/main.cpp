#include <QCoreApplication>

#include <iostream>
#include "network/cserver.h"
#include "thread/cthreadpool.h"
#include <chrono>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    zcell_lib::CServer v;
    if (v.start("127.0.0.1", 5356))
        std::cout << "Server starte" << std::endl;

    return app.exec();
}
