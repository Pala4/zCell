#ifndef CSOCKETTASK_H
#define CSOCKETTASK_H

#include <queue>

#include "thread/cjob_.h"
#include "netdata.h"

class QEventLoop;

namespace zcell_lib {

class CTcpSocket;

class CSocketTask : public CJob_
{
public:
    typedef std::function<void(const net_data_ptr_t&)> hld_on_ready_read_t;
    typedef std::function<void(CSocketTask*)> hld_on_disconnect_t;
    typedef std::function<void(CSocketTask*)> hld_on_destroyed_t;

    CSocketTask(const std::string &address, const uint16_t &port,
                    const CJob_::job_type_t &job_type);
    CSocketTask(const int &socket_descriptor, const CJob_::job_type_t &job_type);

    void set_hld_on_ready_read(const hld_on_ready_read_t &hld_on_ready_read);
    void set_hld_on_disconnect(const hld_on_disconnect_t &hld_on_disconnect);
    void set_hld_on_destroyed(const hld_on_destroyed_t &hld_on_destroyed);

    virtual void run(CWorker *worker) override;

    void send_data(net_data_ptr_t net_data);

    virtual ~CSocketTask() override;
private:
    bool m_use_socket_descriptor = false;
    std::string m_address = "";
    uint16_t m_port = 0;

    std::mutex m_mtx_hld_on_ready_read;
    hld_on_ready_read_t m_hld_on_ready_read = nullptr;
    std::mutex m_mtx_hld_on_disconnect;
    hld_on_disconnect_t m_hld_on_disconnect = nullptr;
    std::mutex m_mtx_hld_on_destroyed;
    hld_on_destroyed_t m_hld_on_destroyed = nullptr;

    QEventLoop *m_event_loop = nullptr;
    int m_socket_descriptor;
    CTcpSocket *m_socket = nullptr;

    std::mutex m_mtx_query_queue;
    std::queue<net_data_ptr_t> m_query_queue;
    net_data_ptr_t next_query();
};

} // namespace zcell_lib

#endif // CSOCKETTASK_H
