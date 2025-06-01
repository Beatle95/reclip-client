#include "communication/connection_impl.h"

#include <cassert>
#include <span>

#include "base/byte_swap.h"
#include "base/log.h"

namespace reclip {
namespace {

struct NetworkHeader {
  uint64_t id;     // Unique id of the message.
  uint16_t type;   // Value of MessageType enum.
  char unused[6];  // Reserved.
};
static_assert(sizeof(NetworkHeader) == 16);

struct NetworkHeaderWithLength {
  uint64_t len;
  NetworkHeader header;
};
static_assert(sizeof(NetworkHeaderWithLength) == 24);

void ParseNetworkHeader(const QByteArray& data, NetworkHeader& header) {
  assert(static_cast<size_t>(data.size()) >= sizeof(NetworkHeader));
  std::memcpy(reinterpret_cast<char*>(&header), data.data(), sizeof(header));
  header.id = ntoh(header.id);
  header.type = ntoh(header.type);
}

}  // namespace

ServerConnectionImpl::ServerConnectionImpl(Connection::Delegate& delegate,
                                           const QString& ip, quint16 port)
    : ip_(ip), port_(port), delegate_(&delegate) {
  connect(&socket_, &QTcpSocket::connected, this,
          &ServerConnectionImpl::OnConnected);
  connect(&socket_, &QTcpSocket::disconnected, this,
          &ServerConnectionImpl::OnDisconnected);
  connect(&socket_, &QTcpSocket::errorOccurred, this,
          &ServerConnectionImpl::OnError);
  connect(&socket_, &QTcpSocket::readyRead, this,
          &ServerConnectionImpl::OnReadyRead);
}

void ServerConnectionImpl::Connect() {
  assert(!connected_);
  socket_.connectToHost(QHostAddress(ip_), port_);
}

void ServerConnectionImpl::Disconnect() { socket_.disconnectFromHost(); }

bool ServerConnectionImpl::SendMessage(uint64_t id, ClientMessageType type,
                                       const QByteArray& data) {
  NetworkHeaderWithLength full_header{
      .len = sizeof(NetworkHeader) + data.size(),
      .header = {.id = id, .type = static_cast<uint16_t>(type), .unused = {}}};
  full_header.len = hton(full_header.len);
  full_header.header.id = hton(full_header.header.id);
  full_header.header.type = hton(full_header.header.type);
  if (socket_.write(reinterpret_cast<const char*>(&full_header),
                    sizeof(full_header)) != sizeof(full_header)) {
    LOG(ERROR) << "Unable to send network header via socket";
    return false;
  }
  if (socket_.write(data) != data.size()) {
    LOG(ERROR) << "Unable to send data via socket";
    return false;
  }
  return true;
}

void ServerConnectionImpl::OnConnected() {
  connected_ = true;
  reassembler_.Clear();
  delegate_->HandleConnected(true);
}

void ServerConnectionImpl::OnDisconnected() {
  connected_ = false;
  delegate_->HandleDisconnected();
}

void ServerConnectionImpl::OnReadyRead() {
  auto data = socket_.readAll();
  if (!data.isEmpty()) {
    reassembler_.Process(data);
  }

  while (reassembler_.HasMessage()) {
    auto msg = reassembler_.PopMessage();
    NetworkHeader header;
    ParseNetworkHeader(msg, header);
    delegate_->HandleReceieved(header.id,
                               static_cast<ServerMessageType>(header.type),
                               msg.mid(sizeof(NetworkHeader)));
  }
}

void ServerConnectionImpl::OnError(QAbstractSocket::SocketError err) {
  if (connected_) {
    LOG(ERROR) << "Connection error occured: " << err;
  } else {
    delegate_->HandleConnected(false);
  }
}

}  // namespace reclip
