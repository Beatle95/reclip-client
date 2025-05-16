#include "communication/connection_impl.h"

#include <cassert>

#include "base/byte_swap.h"
#include "base/log.h"
#include "base/preferences.h"

namespace reclip {
namespace {

struct NetworkHeader {
  uint64_t id;      // Unique id of the message.
  uint16_t type;    // Value of MessageType enum.
};

void ParseNetworkHeader(const QByteArray& data, NetworkHeader& header) {
  assert(static_cast<size_t>(data.size()) >= sizeof(NetworkHeader));
  std::memcpy(reinterpret_cast<char*>(&header), data.data(), sizeof(header));
  header.id = ntoh(header.id);
  header.type = ntoh(header.type);
}

}  // namespace

ServerConnectionImpl::ServerConnectionImpl(Connection::Delegate& delegate)
    : delegate_(&delegate) {
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
  auto& prefs = Preferences::GetInstance();
  socket_.connectToHost(
      QHostAddress(QString::fromStdString(prefs.GetServerIp())),
      prefs.GetServerPort());
}

void ServerConnectionImpl::Disconnect() { socket_.disconnectFromHost(); }

bool ServerConnectionImpl::SendMessage(uint64_t id, ClientMessageType type,
                                       const QByteArray& data) {
  (void)id;
  (void)type;
  (void)data;
  return true;
}

void ServerConnectionImpl::OnConnected() {
  connected_ = true;
  delegate_->HandleConnected(true);
}

void ServerConnectionImpl::OnDisconnected() {
  connected_ = false;
  delegate_->HandleDisconnected();
}

void ServerConnectionImpl::OnReadyRead() {
  // TODO: it is not correct has to be rewritten.
  const auto data = socket_.readAll();
  NetworkHeader header;
  ParseNetworkHeader(data, header);
  delegate_->HandleReceieved(header.id,
                             static_cast<ServerMessageType>(header.type),
                             data.sliced(sizeof(NetworkHeader)));
}

void ServerConnectionImpl::OnError(QAbstractSocket::SocketError err) {
  if (connected_) {
    LOG(ERROR) << "Connection error occured: " << err;
  } else {
    delegate_->HandleConnected(false);
  }
}

}  // namespace reclip
