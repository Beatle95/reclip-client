#include "communication/connection_impl.h"

#include "base/log.h"
#include "base/preferences.h"

namespace reclip {
namespace {

struct NetworkHeader {
  uint64_t length;  // Length of the message without len field.
  uint64_t id;      // Unique id of the message.
  uint16_t type;    // Value of MessageType enum.
};

std::optional<NetworkHeader> ParseNetworkHeader(const QByteArray& data) {
  if (static_cast<size_t>(data.size()) < sizeof(NetworkHeader)) {
    return {};
  }
  // TODO: swap byte order.
  NetworkHeader header;
  std::memcpy(reinterpret_cast<char*>(&header), data.data(), sizeof(header));
  assert(header.length == data.size() - sizeof(header.length));
  return header;
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
  const auto header = ParseNetworkHeader(data);
  if (!header.has_value()) {
    LOG(ERROR) << "Server has sent message with incorrect header";
    return;
  }
  delegate_->HandleReceieved(header->id,
                             static_cast<ServerMessageType>(header->type),
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
