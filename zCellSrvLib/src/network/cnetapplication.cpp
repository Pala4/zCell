#include "cnetapplication.h"

#include <functional>

#include "netdata.h"
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
    m_thread_pool->set_hld_last_job([this](CJobBase *job){process_last_job(job);});
    m_thread_pool->start();
    m_cmd_manamger = new CCmdManager();
    m_cmd_manamger->set_thread_pool(m_thread_pool);
}

bool CNetApplication::send_net_data(net_data_ptr_t net_data)
{
    if (!net_data || (net_data->net_data_id.empty()))
        return false;
    std::lock_guard lg(m_mtx_socket_tasks);
    auto it = m_socket_tasks.find(net_data->net_data_id);
    if (it == m_socket_tasks.end())
        return false;
    CSocketTask *socket_task = it->second;
    socket_task->send_data(std::move(net_data));
    return true;
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
    auto socket_task = new CSocketTask(socket_descriptor, CJob_::jt_ring);
    if (!m_thread_pool->add_job(socket_task)) {
        delete socket_task;
        return std::string();
    }
    if (!reg_socket_task(socket_task)) {
        socket_task->set_job_type(CJob_::jt_linear_auto);
        return std::string();
    }
    return socket_task->id();
}

std::string CNetApplication::add_socket_task(const std::string &address, const uint16_t &port)
{
    if (m_thread_pool == nullptr)
        return std::string();
    auto socket_task = new CSocketTask(address, port, CJob_::jt_ring);
    if (!m_thread_pool->add_job(socket_task)) {
        delete socket_task;
        return std::string();
    }
    if (!reg_socket_task(socket_task)) {
        socket_task->set_job_type(CJob_::jt_linear_auto);
        return std::string();
    }
    return socket_task->id();
}

void CNetApplication::process_last_job(CJobBase *job)
{
}

bool CNetApplication::reg_socket_task(CSocketTask *socket_task)
{
    if ((socket_task == nullptr) || socket_task->id().empty() ||
            (m_socket_tasks.find(socket_task->id()) != m_socket_tasks.end())) {
        return false;
    }
    m_socket_tasks[socket_task->id()] = socket_task;
    socket_task->set_hld_on_ready_read(std::bind(&CNetApplication::on_socket_task_ready_read, this,
                                                 std::placeholders::_1));
    socket_task->set_hld_on_disconnect(std::bind(&CNetApplication::on_socket_task_disconnect, this,
                                                 std::placeholders::_1));
    socket_task->set_hld_on_destroyed(std::bind(&CNetApplication::on_socket_task_destroyed, this,
                                              std::placeholders::_1));
    return true;
}

void CNetApplication::on_socket_task_ready_read(const net_data_ptr_t &net_data)
{
    if (net_data)
        process_net_data(std::move(net_data));
}

void CNetApplication::on_socket_task_disconnect(CSocketTask *socket_task)
{
    if (socket_task == nullptr)
        return;
}

void CNetApplication::on_socket_task_destroyed(CSocketTask *socket_task)
{
    if (socket_task == nullptr)
        return;
    std::lock_guard lg(m_mtx_socket_tasks);
    auto it = m_socket_tasks.find(socket_task->id());
    if (it == m_socket_tasks.end())
        return;
    m_socket_tasks.erase(it);
}
