#include "base/preferences.h"

#include <cassert>

namespace reclip {
Preferences& Preferences::GetInstance() {
  static Preferences instance;
  return instance;
}

void Preferences::SetHostSecret(const HostId& id) {
  assert(!id.empty());
  id_ = id;
}

const HostId& Preferences::GetHostSecret() const { return id_; }

void Preferences::SetServerIp(std::string_view ip) { server_ip_ = ip; }

const std::string& Preferences::GetServerIp() const { return server_ip_; }

void Preferences::SetServerPort(uint16_t port) { server_port_ = port; }

uint16_t Preferences::GetServerPort() { return server_port_; }

}  // namespace reclip
