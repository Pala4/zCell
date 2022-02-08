#include <QCoreApplication>

#include "network/cclient.h"
#include "commands/cintconvertor.h"
#include "commands/cstringconvertor.h"
#include "thread/cworker.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    zcell_lib::CClient client;

    auto cmd_exit = std::make_shared<zcell_lib::CCommand>();
    cmd_exit->set_name("exit").
            set_async(false).
            set_function([&app, &client](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {
        client.done_input();
        QCoreApplication::exit();
    });
    client.add_command(cmd_exit);

    auto cmd_connect = std::make_shared<zcell_lib::CCommand>();
    cmd_connect->set_name("connect").
            set_async(false).
            add_convertor("-a", zcell_lib::convertor<zcell_lib::CStringConvertor>()).
            add_convertor("-p", zcell_lib::convertor<zcell_lib::CIntConvertor>()).
            set_function([&client](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {

        auto client_id = client.connect_to_host(std::any_cast<std::string>(args.at("-a")),
                                                std::any_cast<int>(args.at("-p")));
        if (client_id.empty()) {
            client.output("Error: connection not established!");
        } else {
            client.output("Connection established. Client id (cid): " + client_id);
        }
    });
    client.add_command(cmd_connect);

    auto cmd_send = std::make_shared<zcell_lib::CCommand>();
    cmd_send->set_name("send").
            set_async(true).
            add_convertor("-cid", zcell_lib::convertor<zcell_lib::CStringConvertor>()).
            add_convertor("-cmd", zcell_lib::convertor<zcell_lib::CStringConvertor>()).
            set_function([&client](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {

        auto net_data = std::make_unique<zcell_lib::net_data_t>();
        net_data->net_data_id = std::any_cast<std::string>(args.at("-cid"));
        net_data->msg = std::any_cast<std::string>(args.at("-cmd"));
        if (!client.send_net_data(std::move(net_data))) {
            client.output("Error: data not sended!");
        }
    });
    client.add_command(cmd_send);

    auto cmd_test = std::make_shared<zcell_lib::CCommand>();
    cmd_test->set_name("test").
            set_async(true).
            set_function([&client](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {
        if (job->worker() != nullptr) {
            for (int i = 0; i < 8; ++i) {
                client.output(std::to_string(i) + " from thread wrk id: " +
                              job->id());// std::to_string(job->worker()->id())
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        } else {
            for (int i = 0; i < 10; ++i) {
                client.output(std::to_string(i) + " from unthread");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
    });
    client.add_command(cmd_test);
    client.init_input();

    return app.exec();
}
