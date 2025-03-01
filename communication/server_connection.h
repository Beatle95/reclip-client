#pragma once
#include <string>

#include "base/host_types.h"

namespace reclip {

// TODO: Add timeouts to operations.
class ServerConnection {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void HandleConnect(bool is_connected) = 0;
    virtual void HandleDisconnected() = 0;

    virtual void HandleFullSync(ClipboardData this_host_data,
                                std::vector<HostData> data,
                                bool is_success) = 0;
    virtual void HandleTextSent(bool is_success) = 0;

    virtual void HandleNewHost(const HostId& id, const std::string& name) = 0;
    virtual void HandleNewText(const HostId& id, const std::string& text) = 0;
  };

  virtual ~ServerConnection() = default;

  virtual void Connect(const HostSecret& secret) = 0;
  virtual void Disconnect() = 0;
  virtual void RequestFullSync() = 0;
  virtual void SendText(const std::string& text) = 0;

 protected:
  ServerConnection() = default;
};

}  // namespace reclip
