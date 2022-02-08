#ifndef CCLIENT_H
#define CCLIENT_H

#include <thread>

#include "zCellSrvLib_global.h"
#include "cnetapplication.h"
#include "commands/ccommand.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CClient : public CNetApplication
{
public:
    CClient() = default;

    bool add_command(const command_ptr_t &command);

    std::string connect_to_host(const std::string &address, const uint16_t &port);
    void init_input();
    const bool &is_input_active();
    void done_input();
    void output(const std::string &out, const bool &out_prompt = true);

    virtual ~CClient();
protected:
    virtual void process_net_data(const net_data_ptr_t &net_data);
private:
    std::mutex m_mtx_output;
    std::mutex m_mtx_input_active;
    bool m_input_active = false;
    std::thread m_input_thread;
    std::mutex m_mtx_now_input;
    bool m_now_input = false;
    void set_input_active(const bool &input_active);
    void input_func();
    const bool &is_now_input();
    void set_now_input(const bool &now_input);
};

} // namespace zcell_lib

#endif // CCLIENT_H
