#include "ctcpsocket.h"

using namespace zcell_lib;

/*!
 * \class CTcpSocket
 */
CTcpSocket::CTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &QAbstractSocket::readyRead, this, &CTcpSocket::onReadyRead);
    connect(this, &QAbstractSocket::disconnected, this, &CTcpSocket::onDisconnected);
}

const CTcpSocket::hld_on_ready_read_t &CTcpSocket::hld_on_ready_read() const
{
    return m_hld_on_ready_read;
}

void CTcpSocket::set_hld_on_ready_read(const hld_on_ready_read_t &hld_on_ready_read)
{
    m_hld_on_ready_read = hld_on_ready_read;
}

const CTcpSocket::hld_t &CTcpSocket::hld_on_disconnected() const
{
    return m_hld_on_disconnected;
}

void CTcpSocket::set_hld_on_disconnected(const hld_t &hld_on_disconnected)
{
    m_hld_on_disconnected = hld_on_disconnected;
}

bool CTcpSocket::connectToHost(const std::string &address, const uint16_t &port)
{
    QString addr = QString(address.c_str());
    QHostAddress ha(addr);
    QTcpSocket::connectToHost(ha, port);
    if (!waitForConnected())
        return false;
    return true;
}

void CTcpSocket::send_data(const std::string &data)
{
    if (data.empty())
        return;
    auto data_size = data.size();
    std::string data_ = std::to_string(data_size) + "ds";
    data_.resize(22);
    data_ += data;
    write(data_.c_str(), data_.size());
    waitForBytesWritten();
}

void CTcpSocket::onReadyRead()
{
    std::string data_size_str_;
    data_size_str_.resize(22);
    read(const_cast<char*>(data_size_str_.c_str()), 22);
    uint8_t pos_ds = data_size_str_.find("ds");
    std::string data_size_str = data_size_str_.erase(pos_ds, 22);
    uint64_t data_size = std::stoull(data_size_str);
    std::string data;
    data.resize(data_size);
    read(const_cast<char*>(data.c_str()), data_size);
    if (m_hld_on_ready_read != nullptr) {
        m_hld_on_ready_read(data);
    }
}

void CTcpSocket::onDisconnected()
{
    if (m_hld_on_disconnected != nullptr)
        m_hld_on_disconnected();
}
