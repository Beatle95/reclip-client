#include "core/server.h"

namespace reclip {

Server::Server(Delegate& delegate) : delegate_(&delegate) {}

void Server::OnTextUpdated(const std::string& value) {
  // TODO:
  (void)value;
  (void)delegate_;
}

void Server::RequestFullSync(SyncCallback callback) {
  (void)callback;
  // TODO:
}

}  // namespace reclip
