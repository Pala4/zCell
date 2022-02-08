#ifndef NETDATA_H
#define NETDATA_H

#include <string>
#include <memory>

#include "zCellSrvLib_global.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT net_data_t
{
public:
    int socket_descriptor;
    std::string address;
    uint16_t port;
    std::string net_data_id;
    std::string msg;
    net_data_t() = default;
    ~net_data_t() = default;
};

typedef std::unique_ptr<net_data_t> net_data_ptr_t;

} // namespace zcell_lib

#endif // NETDATA_H
