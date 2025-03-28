#pragma once
#include <string>
#include <string_view>

#include "core/host_types.h"

namespace reclip {

class Preferences {
 public:
  ~Preferences() = default;
  static Preferences& GetInstance();

  // Main preferences.
  void SetHostSecret(const HostId& id);
  const HostId& GetHostSecret() const;

  void SetServerIp(std::string_view ip);
  const std::string& GetServerIp() const;
  void SetServerPort(uint16_t port);
  uint16_t GetServerPort();

 private:
  Preferences() = default;
  HostId id_;
  std::string server_ip_;
  uint16_t server_port_;
};

}  // namespace reclip
