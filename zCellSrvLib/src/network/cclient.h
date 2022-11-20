#ifndef CCLIENT_H
#define CCLIENT_H

#include "zCellSrvLib_global.h"
#include "cnetapplication.h"
#include "commands/ccommand.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CClient : public CNetApplication
{
public:
    CClient() = default;
public:
    bool add_command(const command_ptr_t &command);
    std::string connect_to_host(const std::string &address, const uint16_t &port);
};

} // namespace zcell_lib

#endif // CCLIENT_H
