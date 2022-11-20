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
