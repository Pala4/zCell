#include "csockettask.h"

#include <QEventLoop>

#include "ctcpsocket.h"
#include "cnetapplication.h"

using namespace zcell_lib;

/*!
 * \class CSocketTask
 */
CSocketTask::CSocketTask(CNetApplication *net_app, const std::string &address, const uint16_t &port,
                         const job_type_t &job_type) : CJob(job_type)
{
    m_net_app = net_app;
    m_address = address;
    m_port = port;
}

CSocketTask::CSocketTask(CNetApplication *net_app, const int &socket_descriptor,
                                 const job_type_t &job_type) : CJob(job_type)
{
    m_net_app = net_app;
    m_socket_descriptor = socket_descriptor;
    m_use_socket_descriptor = true;
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
                set_job_type(CJob::jt_linear_auto);
                return;
            }
            m_socket->send_data("zCell server v1.0.0");
        } else {
            if (!m_socket->connectToHost(m_address, m_port)) {
                set_job_type(CJob::jt_linear_auto);
                return;
            }
        }
        m_socket->set_hld_on_ready_read([this](const std::string &data) {
            if (m_net_app != nullptr) {
                auto net_data = std::make_unique<net_data_t>();
                net_data->net_data_id = id();
                net_data->address = m_socket->peerAddress().toString().toLatin1().constData();
                net_data->port = m_socket->peerPort();
                net_data->socket_descriptor = m_socket->socketDescriptor();
                net_data->msg = data;
                m_net_app->receive_net_data(std::move(net_data));
            }
        });
        m_socket->set_hld_on_disconnected([this]() {
            set_job_type(CJob::jt_linear_auto);
            if (m_net_app != nullptr)
                m_net_app->clear_net_data(id());
        });
    }
    if (m_net_app != nullptr) {
        auto net_data_vec = std::move(m_net_app->get_net_data(id()));
        for (const auto &net_data : net_data_vec) {
            if (net_data) {
                if (!net_data->msg.empty())
                    m_socket->send_data(net_data->msg);
            }
        }
    }
    m_event_loop->processEvents();
}

CSocketTask::~CSocketTask()
{
    if (m_event_loop != nullptr)
        delete m_event_loop;
}
