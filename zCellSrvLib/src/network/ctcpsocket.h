#ifndef CTCPSOCKET_H
#define CTCPSOCKET_H

#include "zCellSrvLib_global.h"

#include <QtNetwork/QTcpSocket>

namespace zcell_lib {

class CTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    typedef std::function<void(const std::string &data)> hld_on_ready_read_t;
    typedef std::function<void()> hld_t;

    explicit CTcpSocket(QObject *parent = nullptr);

    const hld_on_ready_read_t &hld_on_ready_read() const;
    void set_hld_on_ready_read(const hld_on_ready_read_t &hld_on_ready_read);
    const hld_t &hld_on_disconnected() const;
    void set_hld_on_disconnected(const hld_t &hld_on_disconnected);

    bool connectToHost(const std::string &address, const uint16_t &port);
    void send_data(const std::string &data);
private:
    hld_on_ready_read_t m_hld_on_ready_read = nullptr;
    hld_t m_hld_on_disconnected = nullptr;
private slots:
    void onReadyRead();
    void onDisconnected();
};

} // namespace zcell_lib

#endif // CTCPSOCKET_H
