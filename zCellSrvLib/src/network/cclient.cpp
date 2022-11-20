#include "cclient.h"

#include "commands/ccmdmanager.h"

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
