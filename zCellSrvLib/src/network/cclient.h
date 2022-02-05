#ifndef CCLIENT_H
#define CCLIENT_H

#include <thread>

#include "zCellSrvLib_global.h"
#include "cnetapplication.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CClient : public CNetApplication
{
public:
    CClient();

    std::string connect_to_host(const std::string &address, const uint16_t &port);
    void init_input();
    const bool &is_input_active();
    void done_input();
    void output(const std::string &out, const bool &out_prompt = true);

    virtual ~CClient();
protected:
    void process_net_data(std::unique_ptr<net_data_t> net_data);
private:
    std::mutex m_mtx_output;
    std::mutex m_mtx_input_active;
    bool m_input_active = false;
    std::thread m_input_thread;
    void set_input_active(const bool &input_active);
    void input_func();
};

} // namespace zcell_lib

#endif // CCLIENT_H
