#pragma once
#include "base/observers_list.h"
#include "communication/server_connection.h"

namespace reclip {

class ServerConnectionImpl : public ServerConnection {
 public:
  explicit ServerConnectionImpl(ServerConnection::Delegate& delegate);
  ~ServerConnectionImpl() override = default;

  // ServerConnection overrides
  void Connect(const HostSecret& secret) override;
  void Disconnect() override;
  void RequestFullSync() override;
  void SendText(const std::string& text) override;

 private:
  ServerConnection::Delegate* delegate_;
};

}  // namespace reclip
