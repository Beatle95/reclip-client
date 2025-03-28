#include "communication/connection_impl.h"

#include "base/log.h"
#include "base/preferences.h"

namespace reclip {

ServerConnectionImpl::ServerConnectionImpl(Connection::Delegate& delegate)
    : delegate_(&delegate) {
  connect(&socket_, &QTcpSocket::connected, this,
          &ServerConnectionImpl::OnConnected);
  connect(&socket_, &QTcpSocket::disconnected, this,
          &ServerConnectionImpl::OnDisconnected);
  connect(&socket_, &QTcpSocket::errorOccurred, this,
          &ServerConnectionImpl::OnError);
}

void ServerConnectionImpl::Connect() {
  auto& prefs = Preferences::GetInstance();
  socket_.connectToHost(
      QHostAddress(QString::fromStdString(prefs.GetServerIp())),
      prefs.GetServerPort());
}

void ServerConnectionImpl::Disconnect() { socket_.disconnectFromHost(); }

bool ServerConnectionImpl::SendMessage(const QByteArray&) { return true; }

void ServerConnectionImpl::OnConnected() {
  connected_ = true;
  delegate_->HandleConnected(true);
}

void ServerConnectionImpl::OnDisconnected() {
  connected_ = false;
  delegate_->HandleDisconnected();
}

void ServerConnectionImpl::OnError(QAbstractSocket::SocketError err) {
  if (connected_) {
    LOG(ERROR) << "Connection error occured: " << err;
  } else {
    delegate_->HandleConnected(false);
  }
}

}  // namespace reclip
