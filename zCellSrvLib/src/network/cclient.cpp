#include "cclient.h"

#include <iostream>

#include "commands/ccmdmanager.h"
#include "thread/cworker.h"
#include "commands/cintconvertor.h"
#include "commands/cstringconvertor.h"
#include "core/cexception.h"

using namespace zcell_lib;

/*!
 * \class CClient
 */
bool CClient::add_command(const command_ptr_t &command)
{
    if (cmd_manager() == nullptr)
        return false;
    return cmd_manager()->add_command(command);
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
    if (is_now_input()) std::cout << '\n';
    std::cout << out;
    if (out_prompt) std::cout << '\n';
    if (is_now_input()) std::cout << "zcell_client>: ";
}

CClient::~CClient()
{
    set_input_active(false);
    if (m_input_thread.joinable())
        m_input_thread.join();
}

void CClient::process_net_data(const net_data_ptr_t &net_data)
{
    output(net_data->msg);
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
        set_now_input(true);
        while (std::cin.get(ch) && ch != '\n') {
            input.append(1, ch);
        }
        set_now_input(false);
        if (cmd_manager() != nullptr) {
            try {
                cmd_manager()->execute(input);
            } catch (CException &ex) {
                output(ex.what());
            }
        }
    }
}

const bool &CClient::is_now_input()
{
    std::lock_guard lg(m_mtx_now_input);
    return m_now_input;
}

void CClient::set_now_input(const bool &now_input)
{
    std::lock_guard lg(m_mtx_now_input);
    m_now_input = now_input;
}
