#include "cnetapplication.h"

#include "thread/cthreadpool.h"
#include "csockettask.h"
#include "commands/ccmdmanager.h"

using namespace zcell_lib;

/*!
 * \class CNetApplication
 */
CNetApplication::CNetApplication()
{
    m_thread_pool = new CThreadPool();
    m_thread_pool->set_hld_last_job([this](CJob *job){process_last_job(job);});
    m_thread_pool->start();
    m_cmd_manamger = new CCmdManager();
    m_cmd_manamger->set_thread_pool(m_thread_pool);
}

void CNetApplication::send_net_data(std::unique_ptr<net_data_t> net_data)
{
    m_mtx_net_datas.lock();
    m_net_datas[net_data->net_data_id].push_back(std::move(net_data));
    m_mtx_net_datas.unlock();
}

void CNetApplication::receive_net_data(std::unique_ptr<net_data_t> net_data)
{
    if (net_data)
        process_net_data(std::move(net_data));
}

std::vector<std::unique_ptr<net_data_t>> CNetApplication::get_net_data(const std::string &net_data_id)
{
    std::lock_guard lg(m_mtx_net_datas);
    auto net_data_it = m_net_datas.find(net_data_id);
    if (net_data_it == m_net_datas.end())
        return std::vector<std::unique_ptr<net_data_t>>();
    auto net_data_vector = std::move(net_data_it->second);
    m_net_datas.erase(net_data_it);
    return net_data_vector;
}

void CNetApplication::clear_net_data(const std::string &net_data_id)
{
    std::lock_guard lg(m_mtx_net_datas);
    auto net_data_it = m_net_datas.find(net_data_id);
    if (net_data_it != m_net_datas.end())
        m_net_datas.erase(net_data_it);
}

CThreadPool *CNetApplication::thread_pool() const
{
    return m_thread_pool;
}

CCmdManager *CNetApplication::cmd_manager() const
{
    return m_cmd_manamger;
}

CNetApplication::~CNetApplication()
{
    if (m_thread_pool != nullptr)
        delete m_thread_pool;
    if (m_cmd_manamger != nullptr)
        delete m_cmd_manamger;
}

std::string CNetApplication::add_socket_task(const int &socket_descriptor)
{
    if (m_thread_pool == nullptr)
        return std::string();
    auto socket_task = new CSocketTask(this, socket_descriptor, CJob::jt_ring);
    if (!m_thread_pool->add_job(socket_task)) {
        delete socket_task;
        return std::string();
    }
    return socket_task->id();
}

std::string CNetApplication::add_socket_task(const std::string &address, const uint16_t &port)
{
    if (m_thread_pool == nullptr)
        return std::string();
    auto socket_task = new CSocketTask(this, address, port, CJob::jt_ring);
    if (!m_thread_pool->add_job(socket_task)) {
        delete socket_task;
        return std::string();
    }
    return socket_task->id();
}

void CNetApplication::process_last_job(CJob *job)
{
}
