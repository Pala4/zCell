#ifndef CSOCKETTASK_H
#define CSOCKETTASK_H

#include "thread/cjob.h"

class QEventLoop;

namespace zcell_lib {

class CNetApplication;
class CTcpSocket;

class CSocketTask : public CJob
{
public:
    CSocketTask(CNetApplication *net_app, const std::string &address, const uint16_t &port,
                    const CJob::job_type_t &job_type);
    CSocketTask(CNetApplication *net_app, const int &socket_descriptor,
                    const CJob::job_type_t &job_type);

    virtual void run(CWorker *worker) override;

    virtual ~CSocketTask() override;
private:
    bool m_use_socket_descriptor = false;
    CNetApplication *m_net_app = nullptr;
    QEventLoop *m_event_loop = nullptr;
    int m_socket_descriptor;
    CTcpSocket *m_socket = nullptr;
    std::mutex m_mtx_queryes;
    std::string m_address = "";
    uint16_t m_port = 0;
};

} // namespace zcell_lib

#endif // CSOCKETTASK_H
