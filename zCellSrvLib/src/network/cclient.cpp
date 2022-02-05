#include "cclient.h"

#include <iostream>

#include <commands/ccmdmanager.h>
#include <thread/cworker.h>

using namespace zcell_lib;

/*!
 * \class CClient
 */
CClient::CClient() : CNetApplication()
{
    cmd_manager()->create_command("connect_to_host", false,
                                  [this](CJob *job, const CJob::args_map_t &args) {
        if (args.find("-a") == args.end())
            return;
        if (args.find("-p") == args.end())
            return;

        auto client_id = connect_to_host(std::any_cast<std::string>(args.at("-a")),
                                         std::stoi(std::any_cast<std::string>(args.at("-p"))));
        if (client_id.empty()) {
            output("Error: connection not established!");
        } else {
            output("Connection established. Client id (cid): " + client_id);
        }
    });
    cmd_manager()->create_command("test", true,
                                  [this](CJob *job, const CJob::args_map_t &args) {
        if (job->worker() != nullptr) {
            for (int i = 0; i < 10; ++i) {
                output(std::to_string(i) + " from thread wrk id: " +
                       std::to_string(job->worker()->id()) + '\n');
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        } else {
            for (int i = 0; i < 10; ++i) {
                output(std::to_string(i) + " from unthread" + '\n');
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
    });
    cmd_manager()->create_command("send_cmd_to_host", false,
                                  [this](CJob *job, const CJob::args_map_t &args) {
        if (args.find("-cid") == args.end())
            return;
        if (args.find("-cmd") == args.end())
            return;
        auto net_data = std::make_unique<zcell_lib::net_data_t>();
        //TODO: what if -cid is not correct?
        net_data->net_data_id = std::any_cast<std::string>(args.at("-cid"));
        net_data->msg = std::any_cast<std::string>(args.at("-cmd"));
        send_net_data(std::move(net_data));
    });
}

std::string CClient::connect_to_host(const std::string &address, const uint16_t &port)
{
    return add_socket_task(address, port);
}

void CClient::init_input()
{
    if (!is_input_active()) {
        set_input_active(true);
        m_input_thread = std::thread(&CClient::input_func, this);
    }
}

const bool &CClient::is_input_active()
{
    std::lock_guard lg(m_mtx_input_active);
    return m_input_active;
}

void CClient::done_input()
{
    set_input_active(false);
}

void CClient::output(const std::string &out, const bool &out_prompt)
{
    std::lock_guard lg(m_mtx_output);
    if (out_prompt) std::cout << '\n' << "zcell_client>: ";
    std::cout << out;
    if (out_prompt) std::cout << "zcell_client>: ";
}

CClient::~CClient()
{
    set_input_active(false);
    if (m_input_thread.joinable())
        m_input_thread.join();
}

void CClient::process_net_data(std::unique_ptr<net_data_t> net_data)
{
    output(net_data->msg + '\n');
}

void CClient::set_input_active(const bool &input_active)
{
    std::lock_guard lg(m_mtx_input_active);
    m_input_active = input_active;
}

void CClient::input_func()
{
    while (true && is_input_active()) {
        output("zcell_client>: ", false);
        std::string input;
        char ch;
        while (std::cin.get(ch) && ch != '\n') {
            input.append(1, ch);
        }
        if (cmd_manager() != nullptr)
            cmd_manager()->execute(input);
    }
}
