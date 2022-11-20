#ifndef CCONCLIENT_H
#define CCONCLIENT_H

#include <thread>

#include "network/cclient.h"

class CConClient : public zcell_lib::CClient
{
public:
    CConClient();
    ~CConClient() override;
public:
    void show_help(const std::string &cmd_name = std::string());
    void set_shell(const std::string &cid);
public:
    void init_input();
    const bool &is_input_active();
    void done_input();
    void output(const std::string &out, const bool &out_prompt = true);
protected:
    void process_net_data(const zcell_lib::net_data_ptr_t &net_data) override;
private:
    std::mutex m_mtx_output;
    std::mutex m_mtx_input_active;
    bool m_input_active = false;
    std::thread m_input_thread;
    std::mutex m_mtx_now_input;
    bool m_now_input = false;
private:
    void set_input_active(const bool &input_active);
    void input_func();
    const bool &is_now_input();
    void set_now_input(const bool &now_input);
};

#endif // CCONCLIENT_H
