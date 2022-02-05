#include "cserver.h"

#include <iostream>

#include "commands/ccmdmanager.h"

using namespace zcell_lib;

/*!
 * \class CTCPServer
 */
CTcpServer::CTcpServer(const incoming_connection_handler_t &incoming_connection_handler,
                       QObject *parent) : QTcpServer(parent)
{
    m_incoming_connection_handler = incoming_connection_handler;
}

void CTcpServer::incomingConnection(qintptr socketDescriptor)
{
    if (m_incoming_connection_handler != nullptr)
        m_incoming_connection_handler(socketDescriptor);
}

/*!
 * \class CServer
 */
CServer::CServer() : CNetApplication()
{
    cmd_manager()->create_command("test", true, [this](CJob *job, const CJob::args_map_t &args) {
        if (args.find("net_data") != args.end()) {
            net_data_t net_data;
            try {
                net_data = std::any_cast<net_data_t>(args.at("net_data"));
            } catch (const std::bad_any_cast& e) {
                return;
            }
            send_net_data(std::make_unique<net_data_t>(net_data));
        }
    });
    m_tcp_server = new CTcpServer([this](int socket_descriptor) {
        add_socket_task(socket_descriptor);
    });
}

bool CServer::start(const char *address, const uint16_t &port)
{
    QString addr = QString(address);
    QHostAddress ha(addr);
    if (m_tcp_server != nullptr)
        return m_tcp_server->listen(ha, port);
    return false;
}

CServer::~CServer()
{
    if (m_tcp_server != nullptr)
        delete m_tcp_server;
}

void CServer::process_net_data(std::unique_ptr<net_data_t> net_data)
{
    if (net_data) {
        if (cmd_manager() != nullptr) {
            auto _net_data = *net_data.get();
            CJob::args_map_t ext_args;
            ext_args["net_data"] = std::make_any<net_data_t>(_net_data);
            cmd_manager()->execute(net_data->msg, ext_args);
        }
     }
}
