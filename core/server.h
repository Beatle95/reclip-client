#pragma once
#include "base/host_types.h"

namespace reclip {

// We will probably add some method signatures in the future.
class Server {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;

    virtual void ProcessSyncData(ClipboardData this_host_data,
                                 std::vector<HostData> data) = 0;
    virtual void ProcessNewHost(const HostId& id, const std::string& name) = 0;
    virtual bool ProcessNewText(const HostId& id, const std::string& text) = 0;
  };

  virtual ~Server() = default;
};

}  // namespace reclip
