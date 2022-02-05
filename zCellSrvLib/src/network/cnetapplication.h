#ifndef CNETAPPLICATION_H
#define CNETAPPLICATION_H

#include <memory>
#include <map>
#include <mutex>

#include "zCellSrvLib_global.h"

namespace zcell_lib {

class CThreadPool;
class CCmdManager;
class CJob;

class net_data_t
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

class ZCELLSRVLIB_EXPORT CNetApplication
{
public:
    CNetApplication();

    void send_net_data(std::unique_ptr<net_data_t> net_data);
    void receive_net_data(std::unique_ptr<net_data_t> net_data);
    std::vector<std::unique_ptr<net_data_t>> get_net_data(const std::string &net_data_id);
    void clear_net_data(const std::string &net_data_id);
    CThreadPool *thread_pool() const;
    CCmdManager *cmd_manager() const;

    virtual ~CNetApplication();
protected:
    virtual void process_net_data(std::unique_ptr<net_data_t> net_data) = 0;
    std::string add_socket_task(const int &socket_descriptor);
    std::string add_socket_task(const std::string &address, const uint16_t &port);
    virtual void process_last_job(CJob *job);
private:
    std::mutex m_mtx_net_datas;
    std::map<std::string, std::vector<std::unique_ptr<net_data_t>>> m_net_datas;
    CThreadPool *m_thread_pool = nullptr;
    CCmdManager *m_cmd_manamger = nullptr;
};

} // namespace zcell_lib

#endif // CNETAPPLICATION_H
