#include "communication/server_connection_impl.h"

namespace reclip {

ServerConnectionImpl::ServerConnectionImpl(ServerConnection::Delegate& delegate)
    : delegate_(&delegate) {}

void ServerConnectionImpl::Connect(const HostSecret& secret) {
  (void)secret;
  delegate_->HandleConnect(true);
}

void ServerConnectionImpl::Disconnect() { delegate_->HandleDisconnected(); }

void ServerConnectionImpl::RequestFullSync() {
  delegate_->HandleFullSync({}, {}, true);
}

void ServerConnectionImpl::SendText(const std::string& text) {
  (void)text;
  delegate_->HandleTextSent(true);
}

}  // namespace reclip
