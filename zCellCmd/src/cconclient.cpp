#include "cconclient.h"

#include <iostream>

#include "commands/ccmdmanager.h"
#include "thread/cworker.h"
#include "commands/cintconvertor.h"
#include "commands/cstringconvertor.h"
#include "core/cexception.h"
#include "commands/cstringconvertor.h"

void CConClient::show_help(const std::string &cmd_name)
{
    zcell_lib::CCmdManager::commands_map_t cmds = cmd_manager()->commands();
    if (cmd_name.empty()) {
        for (const auto &cmd_pair : cmds) {
            zcell_lib::command_ptr_t cmd = cmd_pair.second;
            if (cmd)
                output(cmd->name() + "\t- " + cmd->description());
        }
    } else {
        auto cmd_it = cmds.find(cmd_name);
        if (cmd_it == cmds.end()) {
            output("Command [" + cmd_name + "] not found");
            return;
        }
        output(cmd_it->second->name() + ":\t " + cmd_it->second->usage());
        output("Arguments:");
        foreach (auto arg, cmd_it->second->argument_defs())
            output('\t' + arg.name + ":\t" + arg.usage);
    }
}

void CConClient::set_shell(const std::string &cid)
{

}

void CConClient::init_input()
{
    if (!is_input_active()) {
        set_input_active(true);
        m_input_thread = std::thread(&CConClient::input_func, this);
    }
}

const bool &CConClient::is_input_active()
{
    std::lock_guard lg(m_mtx_input_active);
    return m_input_active;
}

void CConClient::done_input()
{
    set_input_active(false);
}

void CConClient::output(const std::string &out, const bool &out_prompt)
{
    std::lock_guard lg(m_mtx_output);
    if (is_now_input()) std::cout << '\n';
    std::cout << out;
    if (out_prompt) std::cout << '\n';
    if (is_now_input()) std::cout << "zcell_client>: ";
}

CConClient::CConClient()
{
    auto cmd_help = std::make_shared<zcell_lib::CCommand>();
    cmd_help->set_name("help").
            set_async(false).
            set_description("Help").
            set_usage(std::string() + "Show help for command/commands.\nhelp [-cmd <command>]\n" +
                      "-cmd <command> - show help topic about command. If dropped, then showing " +
                      "short descriptions for all commands.").
            add_argument_def("-cmd", zcell_lib::convertor<zcell_lib::CStringConvertor>(), false,
                             "Show help topic about command. Optional argument.").
            set_function([this](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {
                show_help(std::any_cast<std::string>(args.at("-cmd")));
            });
    add_command(cmd_help);

    auto cmd_connect = std::make_shared<zcell_lib::CCommand>();
    cmd_connect->set_name("connect").
            set_async(false).
            set_description("Connect to remote server").
            set_usage(std::string() +
                      "Connect to remote server.\nconnect -a <server address> -p <port>").
            add_argument_def("-a", zcell_lib::convertor<zcell_lib::CStringConvertor>(), true,
                             "Remote server address. E.g. 127.0.0.1").
            add_argument_def("-p", zcell_lib::convertor<zcell_lib::CIntConvertor>(), true,
                             "Remote server access port. [0..65535]").
            set_function([this](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {
                auto client_id = connect_to_host(std::any_cast<std::string>(args.at("-a")),
                                                 std::any_cast<int>(args.at("-p")));
                if (client_id.empty()) {
                    output("Error: connection not established!");
                } else {
                    output("Connection established. Client id (cid): " + client_id);
                }
            });
    add_command(cmd_connect);

    auto cmd_send = std::make_shared<zcell_lib::CCommand>();
    cmd_send->set_name("send").
            set_async(true).
            add_argument_def("-cid", zcell_lib::convertor<zcell_lib::CStringConvertor>()).
            add_argument_def("-cmd", zcell_lib::convertor<zcell_lib::CStringConvertor>()).
            set_function([this](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {
                auto net_data = std::make_unique<zcell_lib::net_data_t>();
                net_data->net_data_id = std::any_cast<std::string>(args.at("-cid"));
                net_data->msg = std::any_cast<std::string>(args.at("-cmd"));
                if (!send_net_data(std::move(net_data))) {
                    output("Error: data not sended!");
                }
            });
    add_command(cmd_send);

    auto cmd_shell = std::make_shared<zcell_lib::CCommand>();
    cmd_shell->set_name("shell").
            set_async(false).
            add_argument_def("-cid", zcell_lib::convertor<zcell_lib::CStringConvertor>()).
            set_function([this](zcell_lib::CJobBase *job,
                         const zcell_lib::CJob_::args_map_t &args) {
                set_shell(std::any_cast<std::string>(args.at("-cid")));
            });
    add_command(cmd_shell);
}

CConClient::~CConClient()
{
    set_input_active(false);
    if (m_input_thread.joinable())
        m_input_thread.join();
}

void CConClient::process_net_data(const zcell_lib::net_data_ptr_t &net_data)
{
    output(net_data->msg);
}

void CConClient::set_input_active(const bool &input_active)
{
    std::lock_guard lg(m_mtx_input_active);
    m_input_active = input_active;
}

void CConClient::input_func()
{
    while (true && is_input_active()) {
        output("zcell_client>: ", false);
        std::string input;
        char ch;
        set_now_input(true);
        while (std::cin.get(ch) && ch != '\n') {
            input.append(1, ch);
        }
        set_now_input(false);
        if (cmd_manager() != nullptr) {
            try {
                cmd_manager()->execute(input);
            } catch (zcell_lib::CException &ex) {
                output(ex.what());
            }
        }
    }
}

const bool &CConClient::is_now_input()
{
    std::lock_guard lg(m_mtx_now_input);
    return m_now_input;
}

void CConClient::set_now_input(const bool &now_input)
{
    std::lock_guard lg(m_mtx_now_input);
    m_now_input = now_input;
}
