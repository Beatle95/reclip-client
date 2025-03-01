#pragma once
#include "base/host_types.h"

namespace reclip {

class ServerDelegate {
 public:
  virtual ~ServerDelegate() = default;

  virtual void ProcessSyncData(ClipboardData this_host_data,
                               std::vector<HostData> data) = 0;
  virtual void ProcessNewHost(const HostId& id, const std::string& name) = 0;
  // This function must return false, if Delegate does not recognized HostId.
  virtual bool ProcessNewText(const HostId& id, const std::string& text) = 0;
};

}  // namespace reclip
