#include <QCoreApplication>

#include "network/cclient.h"
#include <chrono>
#include <thread>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    zcell_lib::CClient client;
    client.init_input();

//    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

//    auto client_id = client.connectToHost("127.0.0.1", 5356);

//    auto net_data = std::make_unique<zcell_lib::net_data_t>();
//    net_data->net_data_id = client_id;
//    net_data->msg = "Query";

//    client.send_net_data(std::move(net_data));

    return app.exec();
}
