#pragma once
#include <functional>
#include <string>
#include <optional>

#include "core/host_types.h"

namespace reclip {

// This interface describes Server->Client calls.
class ServerDelegate {
 public:
  virtual ~ServerDelegate() = default;
  virtual void OnFullSync(HostData, std::vector<HostData>) = 0;
  virtual void HostConnected(const HostId& id) = 0;
  virtual void HostDisconnected(const HostId& id) = 0;
  virtual void HostTextAdded(const HostId& id, const std::string& text) = 0;
  virtual void HostSynced(HostData data) = 0;
};

// This interface describes Client->Server calls.
class Server {
 public:
  using HostSyncCallback = std::function<void(std::optional<HostData>)>;

  virtual ~Server() = default;
  virtual void RequestHostSync(const HostId& id, HostSyncCallback callback) = 0;
  virtual void SyncThisHost(const HostData& data) = 0;
  virtual void AddThisHostText(const std::string& text) = 0;
};

}  // namespace reclip
