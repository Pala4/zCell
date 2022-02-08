#include "csockettask.h"

#include <QEventLoop>

#include "ctcpsocket.h"

using namespace zcell_lib;

/*!
 * \class CSocketTask
 */
CSocketTask::CSocketTask(const std::string &address, const uint16_t &port,
                         const job_type_t &job_type) : CJob_(job_type)
{
    m_address = address;
    m_port = port;
}

CSocketTask::CSocketTask(const int &socket_descriptor, const job_type_t &job_type) : CJob_(job_type)
{
    m_socket_descriptor = socket_descriptor;
    m_use_socket_descriptor = true;
}

void CSocketTask::set_hld_on_ready_read(const hld_on_ready_read_t &hld_on_ready_read)
{
    std::lock_guard lg(m_mtx_hld_on_ready_read);
    m_hld_on_ready_read = hld_on_ready_read;
}

void CSocketTask::set_hld_on_disconnect(const hld_on_disconnect_t &hld_on_disconnect)
{
    std::lock_guard lg(m_mtx_hld_on_disconnect);
    m_hld_on_disconnect = hld_on_disconnect;
}

void CSocketTask::set_hld_on_destroyed(const hld_on_destroyed_t &hld_on_destroyed)
{
    std::lock_guard lg(m_mtx_hld_on_destroyed);
    m_hld_on_destroyed = hld_on_destroyed;
}

void CSocketTask::run(CWorker *worker)
{
    if (m_event_loop == nullptr)
        m_event_loop = new QEventLoop();
    m_event_loop->processEvents();
    if (m_socket == nullptr) {
        m_socket = new CTcpSocket(m_event_loop);
        if (m_use_socket_descriptor) {
            if (!m_socket->setSocketDescriptor(m_socket_descriptor)) {
                set_job_type(CJob_::jt_linear_auto);
                return;
            }
            m_socket->send_data("zCell server v1.0.0");
        } else {
            if (!m_socket->connectToHost(m_address, m_port)) {
                set_job_type(CJob_::jt_linear_auto);
                return;
            }
        }
        m_socket->set_hld_on_ready_read([this](const std::string &data) {
            auto net_data = std::make_unique<net_data_t>();
            net_data->net_data_id = id();
            net_data->address = m_socket->peerAddress().toString().toLatin1().constData();
            net_data->port = m_socket->peerPort();
            net_data->socket_descriptor = m_socket->socketDescriptor();
            net_data->msg = data;
            if (m_hld_on_ready_read != nullptr)
                m_hld_on_ready_read(std::move(net_data));
        });
        m_socket->set_hld_on_disconnected([this]() {
            set_job_type(CJob_::jt_linear_auto);
            if (m_hld_on_disconnect != nullptr)
                m_hld_on_disconnect(this);
        });
    }    
    auto net_data = next_query();
    if (net_data)
        m_socket->send_data(net_data->msg);
    m_event_loop->processEvents();
}

void CSocketTask::send_data(net_data_ptr_t net_data)
{
    if (net_data) {
        if (!net_data->msg.empty())
            m_mtx_query_queue.lock();
            m_query_queue.push(std::move(net_data));
            m_mtx_query_queue.unlock();
    }
}

CSocketTask::~CSocketTask()
{
    if (m_hld_on_destroyed != nullptr)
        m_hld_on_destroyed(this);
    if (m_event_loop != nullptr)
        delete m_event_loop;
}

net_data_ptr_t CSocketTask::next_query()
{
    net_data_ptr_t net_data;
    std::lock_guard lg(m_mtx_query_queue);
    if (m_query_queue.empty())
        return net_data;
    net_data = std::move(m_query_queue.front());
    m_query_queue.pop();
    return net_data;
}
