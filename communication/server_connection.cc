#include "communication/server_connection.h"

#include "communication/server_connection_impl.h"

namespace reclip {
std::unique_ptr<ServerConnection> ServerConnection::Create(Delegate& delegate) {
  return std::make_unique<ServerConnectionImpl>(delegate);
}

}  // namespace reclip
