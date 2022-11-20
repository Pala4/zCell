#include <QCoreApplication>

#include <iostream>
#include <memory>

#include "network/cserver.h"
#include "thread/cthreadpool.h"
#include "commands/ccmdmanager.h"
#include "commands/ccommand.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    zcell_lib::CServer v;
    auto cmd_test = std::make_shared<zcell_lib::CCommand>();
    cmd_test->set_name("test").
            set_async(true).
            set_function([&v](zcell_lib::CJobBase *job, const zcell_lib::CJobBase::args_map_t &args) {
        if (args.find("net_data") != args.end()) {
            zcell_lib::net_data_t net_data;
            try {
                net_data = std::any_cast<zcell_lib::net_data_t>(args.at("net_data"));
            } catch (const std::bad_any_cast& e) {
                return;
            }
            v.send_net_data(std::make_unique<zcell_lib::net_data_t>(net_data));
        }
    });
    v.cmd_manager()->add_command(cmd_test);

    std::string srv_address = "127.0.0.1";
    uint16_t srv_port = 5356;
    if (v.start("127.0.0.1", 5356))
        std::printf("Server started at %s:%d", srv_address.c_str(), srv_port);

    return app.exec();
}
