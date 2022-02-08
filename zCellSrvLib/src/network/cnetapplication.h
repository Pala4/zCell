#ifndef CNETAPPLICATION_H
#define CNETAPPLICATION_H

#include <memory>
#include <map>
#include <mutex>

#include "zCellSrvLib_global.h"
#include "netdata.h"

namespace zcell_lib {

class CThreadPool;
class CCmdManager;
class CJobBase;
class CSocketTask;

class ZCELLSRVLIB_EXPORT CNetApplication
{
public:
    CNetApplication();

    bool send_net_data(net_data_ptr_t net_data);
//    void receive_net_data(const net_data_ptr_t &net_data);
//    std::vector<std::unique_ptr<net_data_t>> get_net_data(const std::string &net_data_id);
//    void clear_net_data(const std::string &net_data_id);
    CThreadPool *thread_pool() const;
    CCmdManager *cmd_manager() const;

    virtual ~CNetApplication();
protected:
    virtual void process_net_data(const net_data_ptr_t &net_data) = 0;
    std::string add_socket_task(const int &socket_descriptor);
    std::string add_socket_task(const std::string &address, const uint16_t &port);
    virtual void process_last_job(CJobBase *job);
private:
    CThreadPool *m_thread_pool = nullptr;
    CCmdManager *m_cmd_manamger = nullptr;
    std::mutex m_mtx_socket_tasks;
    std::map<std::string, CSocketTask*> m_socket_tasks;
    bool reg_socket_task(CSocketTask *socket_task);
    void on_socket_task_ready_read(const net_data_ptr_t &net_data);
    void on_socket_task_disconnect(CSocketTask *socket_task);
    void on_socket_task_destroyed(CSocketTask *socket_task);
};

} // namespace zcell_lib

#endif // CNETAPPLICATION_H
